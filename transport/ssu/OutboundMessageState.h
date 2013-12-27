#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "../../datatypes/ByteArray.h"

#include "PacketBuilder.h"

namespace i2pcpp {
    namespace SSU {
        class OutboundMessageState {
            public:
                OutboundMessageState(uint32_t msgId, ByteArray const &data);
                OutboundMessageState(OutboundMessageState &&) = default;

                const PacketBuilder::FragmentPtr getNextFragment();
                const PacketBuilder::FragmentPtr getNextUnackdFragment() const;
                void markFragmentSent(const uint8_t fragNum);
                void markFragmentAckd(const uint8_t fragNum);
                bool allFragmentsSent() const;
                bool allFragmentsAckd() const;

                uint32_t getMsgId() const;
                void incrementTries();
                uint8_t getTries() const;

                void setTimer(std::unique_ptr<boost::asio::deadline_timer> t);
                boost::asio::deadline_timer& getTimer();

            private:
                void fragment();

                uint32_t m_msgId;
                ByteArray m_data;
                std::vector<PacketBuilder::FragmentPtr> m_fragments;
                uint8_t m_tries = 0;

                /* Bit 1 = received (acknowledged)
                 * Bit 0 = sent (not yet acknowledged)
                 */
                boost::dynamic_bitset<> m_fragmentStates;

                std::unique_ptr<boost::asio::deadline_timer> m_timer;
        };

        typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
    }
}

#endif
