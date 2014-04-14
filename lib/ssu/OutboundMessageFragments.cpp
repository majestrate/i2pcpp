/**
 * @file OutboundMessageFragments.cpp
 * @brief Implements OutboundMessageFragments.h
 */
#include "OutboundMessageFragments.h"

#include "../../include/i2pcpp/transports/SSU.h"

#include <i2pcpp/util/make_unique.h>

namespace i2pcpp {
    namespace SSU {
        OutboundMessageFragments::OutboundMessageFragments(UDPTransport &transport) :
            m_transport(transport) {}

        void OutboundMessageFragments::sendData(PeerState const &ps, uint32_t const msgId, ByteArray const &data)
        {
            auto timer = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 2));
            timer->async_wait(boost::bind(&OutboundMessageFragments::timerCallback, this, boost::asio::placeholders::error, ps, msgId));

            OutboundMessageState oms(msgId, data);
            oms.setTimer(std::move(timer));

            std::lock_guard<std::mutex> lock(m_mutex);
            uint32_t tmp = msgId;
            m_states.emplace(std::make_pair(std::move(tmp), std::move(oms)));

            m_transport.m_ios.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, msgId));
        }

        void OutboundMessageFragments::delState(const uint32_t msgId)
        {
            m_states.erase(msgId);
        }

        void OutboundMessageFragments::sendDataCallback(PeerState ps, uint32_t const msgId)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto itr = m_states.find(msgId);
            if(itr != m_states.end()) {
                OutboundMessageState& oms = itr->second;

                std::vector<PacketBuilder::FragmentPtr> fragList;
                fragList.push_back(oms.getNextFragment());

                oms.markFragmentSent(fragList[0]->fragNum);

                PacketPtr p = PacketBuilder::buildData(ps.getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
                p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
                m_transport.sendPacket(p);

                if(!oms.allFragmentsSent())
                    m_transport.m_ios.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, msgId));
            }
        }

        void OutboundMessageFragments::timerCallback(const boost::system::error_code& e, PeerState ps, uint32_t const msgId)
        {
            if(!e) {
                std::lock_guard<std::mutex> lock(m_mutex);

                auto itr = m_states.find(msgId);
                if(itr != m_states.end()) {
                    OutboundMessageState& oms = itr->second;

                    if(oms.getTries() > 5) {
                        m_states.erase(msgId);
                        return;
                    }

                    PacketBuilder::FragmentPtr frag = oms.getNextUnackdFragment();
                    if(frag) {
                        std::vector<PacketBuilder::FragmentPtr> fragList;
                        fragList.push_back(frag);

                        oms.markFragmentSent(fragList[0]->fragNum);

                        PacketPtr p = PacketBuilder::buildData(ps.getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
                        p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
                        m_transport.sendPacket(p);

                        oms.incrementTries();

                        oms.getTimer().expires_at(oms.getTimer().expires_at() + boost::posix_time::time_duration(0, 0, 2));
                        oms.getTimer().async_wait(boost::bind(&OutboundMessageFragments::timerCallback, this, boost::asio::placeholders::error, ps, msgId));
                    }
                }
            }
        }
    }
}
