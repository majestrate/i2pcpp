/**
 * @file OutboundMessageState.cpp
 * @brief Implements OutboundMessageState.h
 */
#include "OutboundMessageState.h"

#include "../../Log.h"

namespace i2pcpp {
    namespace SSU {
        OutboundMessageState::OutboundMessageState(uint32_t msgId, ByteArray const &data) :
            m_msgId(msgId),
            m_data(data),
            m_fragments() {}

        void OutboundMessageState::fragment()
        {
            constexpr size_t maxFragmentSize = 1120;

            auto dataItr = m_data.cbegin();
            auto end = m_data.cend();

            if(m_data.size() > 16383 * 127)
                throw std::runtime_error("Outbound packet too large");

            size_t step, i = 0;
            while(dataItr < end) {
                step = std::min(
                    static_cast<std::size_t>(std::distance(dataItr, end)),
                    maxFragmentSize
                );

                auto f = std::make_shared<PacketBuilder::Fragment>();
                f->msgId = m_msgId;
                f->fragNum = i++;
                f->isLast = (step < maxFragmentSize);
                f->data = ByteArray(dataItr, dataItr + step);

                m_fragments.push_back(std::make_pair(f, FragmentFlags()));

                dataItr += step;
            }
        }

        const PacketBuilder::FragmentPtr OutboundMessageState::getNextFragment()
        {
            if(!m_fragments.size())
                fragment();

            for(const auto& fs : m_fragments) {
                if(!fs.second.sent)
                    return fs.first;
            }
            return PacketBuilder::FragmentPtr();
        }

        const PacketBuilder::FragmentPtr OutboundMessageState::getNextUnackdFragment() const
        {
            for(const auto& fs : m_fragments) {
                if(!fs.second.ackd)
                    return fs.first;
            }
            return PacketBuilder::FragmentPtr();
        }

        void OutboundMessageState::markFragmentSent(const uint8_t fragNum)
        {
            if(fragNum >= m_fragments.size())
                return;

            m_fragments[fragNum].second.sent = true;
        }

        void OutboundMessageState::markFragmentAckd(const uint8_t fragNum)
        {

            if(fragNum >= m_fragments.size())
                return;

            m_fragments[fragNum].second.ackd = true;
        }

        bool OutboundMessageState::allFragmentsSent() const
        {
            return std::all_of(
                m_fragments.begin(), m_fragments.end(),
                [](const FragmentState& fs) -> bool{
                    return fs.second.sent; 
                }
            );
        }

        bool OutboundMessageState::allFragmentsAckd() const
        {
            return std::all_of(
                m_fragments.begin(), m_fragments.end(),
                [](const FragmentState& fs) -> bool{
                    return fs.second.ackd; 
                }
            );
        }

        uint32_t OutboundMessageState::getMsgId() const
        {
            return m_msgId;
        }

        void OutboundMessageState::incrementTries()
        {
            ++m_tries;
        }

        uint8_t OutboundMessageState::getTries() const
        {
            return m_tries;
        }

        void OutboundMessageState::setTimer(std::unique_ptr<boost::asio::deadline_timer> t)
        {
            m_timer = std::move(t);
        }

        boost::asio::deadline_timer& OutboundMessageState::getTimer()
        {
            return *m_timer;
        }
    }
}
