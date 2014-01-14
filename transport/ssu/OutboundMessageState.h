/**
 * @file OutboundMessageState.h
 * @brief Defines the OutboundMessageState class.
 */
#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "../../datatypes/ByteArray.h"

#include "PacketBuilder.h"

namespace i2pcpp {
    namespace SSU {
        /**
         * Stores the state of a message that has been sent (partially or
         *  fully) or is to be send.
         */
        class OutboundMessageState {
            public:
                OutboundMessageState(uint32_t msgId, ByteArray const &data);
                OutboundMessageState(OutboundMessageState &&) = default;

                /**
                 * @return a pointer to the next fragment that hasn't been sent
                 *  yet.
                 */
                const PacketBuilder::FragmentPtr getNextFragment();

                /**
                 * @return a pointer to the next fragment that hasn't been ACK'd.
                 */
                const PacketBuilder::FragmentPtr getNextUnackdFragment() const;

                /**
                 * Marks the fragment given by its id \a fragNum as sent.
                 */
                void markFragmentSent(const uint8_t fragNum);

                /**
                 * Marks the fragment given by its id \a fragNum as sent.
                 */
                void markFragmentAckd(const uint8_t fragNum);

                /**
                 * @return true if all fragments have been sent, false otherwise
                 */
                bool allFragmentsSent() const;

                /**
                 * @return true if all fragments have been ACK'd, false otherwise
                 */
                bool allFragmentsAckd() const;

                /**
                 * @return the ID of this message
                 */
                uint32_t getMsgId() const;

                /**
                 * Increases the amount of times we tried to send.
                 */
                void incrementTries();

                /**
                 * @return the amount of times we tried to send.
                 */
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
