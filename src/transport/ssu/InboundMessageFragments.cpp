/**
 * @file InboundMessageFragments.cpp
 * @brief Implements InboundMessageFragments.h.
 */
#include "InboundMessageFragments.h"

#include <string>
#include <bitset>
#include <iomanip>

#include <botan/pipe.h>
#include <botan/filters.h>

#include <i2pcpp/util/make_unique.h>

#include "../UDPTransport.h"

#include "InboundMessageState.h"

namespace i2pcpp {
    namespace SSU {
        InboundMessageFragments::InboundMessageFragments(UDPTransport &transport) :
            m_transport(transport),
            m_log(boost::log::keywords::channel = "IMF") {}

        void InboundMessageFragments::receiveData(RouterHash const &rh, ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", rh);

            if(std::distance(end, begin) < 1) throw std::runtime_error("malformed SSU data message");
            std::bitset<8> flag = *(begin++);

            if(flag[7]) {
                if(std::distance(end, begin) < 1) throw std::runtime_error("malformed SSU data message ACK field");
                unsigned char numAcks = *(begin++);
                if(std::distance(end, begin) < (numAcks * 4)) throw std::runtime_error("malformed SSU data message ACK field");

                while(numAcks--) {
                    uint32_t msgId = parseUint32(begin);

                    std::lock_guard<std::mutex> lock(m_transport.m_omf.m_mutex);
                    m_transport.m_omf.delState(msgId);
                }
            }

            if(flag[6]) {

                unsigned char numFields = *(begin++);
                while(numFields--) {
                    uint32_t msgId = parseUint32(begin);

                    // Read ACK bitfield (1 byte)
                    std::lock_guard<std::mutex> lock(m_transport.m_omf.m_mutex);
                    auto itr = m_transport.m_omf.m_states.find(msgId);
                    uint8_t byteNum = 0;
                    do {
                        uint8_t byte = *begin;
                        for(int i = 6, j = 0; i >= 0; i--, j++) {
                            // If the bit is 1, the fragment has been received
                            if(byte & (1 << i)) {
                                if(itr != m_transport.m_omf.m_states.end())
                                    itr->second.markFragmentAckd((byteNum * 7) + j);
                            }
                        }

                        ++byteNum;
                    // If the low bit is 1, another bitfield follows
                    } while(*(begin++) & (1 << 7));

                    if(itr != m_transport.m_omf.m_states.end() && itr->second.allFragmentsAckd())
                        m_transport.m_omf.delState(msgId);
                }
            }

            if(std::distance(end, begin) < 1) throw std::runtime_error("malformed SSU data message");
            unsigned char numFragments = *(begin++);
            I2P_LOG(m_log, debug) << "number of fragments: " << std::to_string(numFragments);

            for(int i = 0; i < numFragments; i++) {
                if(std::distance(end, begin) < 7) throw std::runtime_error("malformed SSU data message");
                uint32_t msgId = parseUint32(begin);
                I2P_LOG(m_log, debug) << "fragment[" << i << "] message id: " << std::hex << msgId << std::dec;

                uint32_t fragInfo = (begin[0] << 16) | (begin[1] << 8) | (begin[2]);
                begin += 3;

                uint16_t fragNum = fragInfo >> 17;
                I2P_LOG(m_log, debug) << "fragment[" << i << "] fragment #: " << fragNum;

                bool isLast = (fragInfo & 0x010000);
                I2P_LOG(m_log, debug) << "fragment[" << i << "] isLast: " << isLast;

                uint16_t fragSize = fragInfo & ((1 << 14) - 1);
                I2P_LOG(m_log, debug) << "fragment[" << i << "] size: " << fragSize;

                if(std::distance(end, begin) < fragSize) throw std::runtime_error("malformed SSU data message");
                ByteArray fragData(begin, begin + fragSize);
                I2P_LOG(m_log, debug) << "fragment[" << i << "] data: " << fragData;

                std::lock_guard<std::mutex> lock(m_mutex);
                auto itr = m_states.get<0>().find(msgId);
                if(itr == m_states.get<0>().end()) {
                    InboundMessageState ims(rh, msgId);
                    ims.addFragment(fragNum, fragData, isLast);

                    checkAndPost(msgId, ims);
                    addState(msgId, rh, std::move(ims));
                } else {
                    m_states.get<0>().modify(itr, AddFragment(fragNum, fragData, isLast));

                    checkAndPost(msgId, itr->state);
                }
            }
        }

        void InboundMessageFragments::addState(const uint32_t msgId, const RouterHash &rh, InboundMessageState ims)
        {
            ContainerEntry sc(std::move(ims));
            sc.msgId = msgId;
            sc.hash = rh;

            auto timer = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 10));
            timer->async_wait(boost::bind(&InboundMessageFragments::timerCallback, this, boost::asio::placeholders::error, msgId));
            sc.timer = std::move(timer);

            m_states.insert(std::move(sc));
        }

        void InboundMessageFragments::delState(const uint32_t msgId)
        {
            m_states.get<0>().erase(msgId);
        }

        void InboundMessageFragments::timerCallback(const boost::system::error_code& e, const uint32_t msgId)
        {
            if(!e) {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_states.get<0>().erase(msgId);
            }
        }

        inline void InboundMessageFragments::checkAndPost(const uint32_t msgId, InboundMessageState const &ims)
        {
            if(ims.allFragmentsReceived()) {
                const ByteArray& data = ims.assemble();
                if(data.size())
                    m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_receivedSignal), ims.getRouterHash(), msgId, data));
            }
        }

        InboundMessageFragments::ContainerEntry::ContainerEntry(InboundMessageState ims) :
            state(std::move(ims)) {}

        InboundMessageFragments::AddFragment::AddFragment(const uint8_t fragNum, ByteArray const &data, bool isLast) :
            m_fragNum(fragNum),
            m_data(data),
            m_isLast(isLast) {}

        void InboundMessageFragments::AddFragment::operator()(ContainerEntry &ce)
        {
            ce.state.addFragment(m_fragNum, m_data, m_isLast);
        }
    }
}
