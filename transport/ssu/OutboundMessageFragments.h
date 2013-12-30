#ifndef SSUOUTBOUNDMESSAGEFRAGMENTS_H
#define SSUOUTBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
    class UDPTransport;

    namespace SSU {
        class OutboundMessageFragments {
            friend class InboundMessageFragments;

            public:
                OutboundMessageFragments(UDPTransport &transport);
                OutboundMessageFragments(const OutboundMessageFragments &) = delete;
                OutboundMessageFragments& operator=(OutboundMessageFragments &) = delete;

                void sendData(PeerState const &ps, uint32_t const msgId, ByteArray const &data);

            private:
                void delState(const uint32_t msgId);
                void sendDataCallback(PeerState ps, uint32_t const msgId);
                void timerCallback(const boost::system::error_code& e, PeerState ps, uint32_t const msgId);

                std::map<uint32_t, SSU::OutboundMessageState> m_states;

                mutable std::mutex m_mutex;

                UDPTransport &m_transport;
        };
    }
}

#endif
