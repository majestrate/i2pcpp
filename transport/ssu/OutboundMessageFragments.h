/**
 * @file OutboundMessageFragments.h
 * @brief Defines the i2pcpp::SSU::OutboundMessageFragments class. 
 */
#ifndef SSUOUTBOUNDMESSAGEFRAGMENTS_H
#define SSUOUTBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
    class UDPTransport;

    namespace SSU {
        /**
         * Manages (fragments) of messages sent by this router.
         */
        class OutboundMessageFragments {
            friend class InboundMessageFragments;

            public:
                /**
                 * Constructs from a reference to the i2pcpp::UDPTransport object.
                 */
                OutboundMessageFragments(UDPTransport &transport);
                OutboundMessageFragments(const OutboundMessageFragments &) = delete;
                OutboundMessageFragments& operator=(OutboundMessageFragments &) = delete;

                /**
                 * Writes a message given by its \a msgId to the i2pcpp::SSU::PeerState
                 *  \a ps.
                 */
                void sendData(PeerState const &ps, uint32_t const msgId, ByteArray const &data);

            private:
                /**
                 * Adds a state to the states std::map, OutboundMessageFragments::m_states.
                 * @param msgId the message ID of the state that is to be added.
                 * @param oms the state object to be added
                 */
                void addState(PeerState const & ps, uint32_t const msgId, OutboundMessageState &oms);

                /**
                 * Removes a state from the states std::map, OutboundMessageFragments::m_states.
                 * @param msgId the message ID of the state to be deleted
                 */
                void delState(const uint32_t msgId);
                
                /**
                 * Iterates over all of the states and sends the (incomplete) messages
                 *  using the i2pcpp::UDPTranport. If not all of the fragments have
                 *  been sent, posts a message to the IO service to call this function
                 *  again.
                 */
                void sendDataCallback(PeerState ps, uint32_t const msgId);

                /**
                 * Called when the timer's deadline expires. Tries to resend the
                 *  next un ACK'd fragment for the message. If it this had been
                 *  tried more than 5 times before, removes the timer for the
                 *  given \a msgId.
                 */
                void timerCallback(const boost::system::error_code& e, PeerState ps, uint32_t const msgId);

                std::map<uint32_t, SSU::OutboundMessageState> m_states;
                std::map<uint32_t, std::unique_ptr<boost::asio::deadline_timer>> m_timers;

                mutable std::mutex m_mutex;

                UDPTransport &m_transport;
        };
    }
}

#endif
