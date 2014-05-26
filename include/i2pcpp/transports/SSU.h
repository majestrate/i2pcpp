/**
 * @file SSU.h
 * @brief Defines the i2pcpp::SSU::SSU class.
 */
#ifndef SSU_H
#define SSU_H

#include <i2pcpp/Transport.h>

namespace Botan { class DSA_PrivateKey; }

namespace i2pcpp {
    class Endpoint;
    class RouterIdentity;

    namespace SSU {
        struct Context;

        class SSU : public Transport {
            friend Context;

            public:
                /**
                 * Constructs an SSU transport given a private DSA key and
                 * RouterIdentity. The DSA key is used for signing and the
                 * RouterIdentity key (ElGamal) is used for encryption.
                 */
                SSU(std::shared_ptr<Botan::DSA_PrivateKey> const &dsaPrivKey, RouterIdentity const &ri);
                ~SSU();

                /**
                 * Starts the transport. That is, binds the socket to the
                 *  i2pcpp::Endpoint and then starts receiving data.
                 * @param ep the i2pcpp::Endpoint to listen on
                 */
                void start(Endpoint const &ep);

                /**
                 * Iterates over all addresses listed in the i2pcpp::RouterInfo, and
                 *  attempts to establish a session with the first one that has SSU
                 *  as the transport.
                 */
                void connect(RouterInfo const &ri);

                void send(RouterHash const &rh, uint32_t msgId, ByteArray const &data);

                /**
                 * Disconnects the peer given by i2pcpp::RouterHash \a rh.
                 */
                void disconnect(RouterHash const &rh);

                uint32_t numPeers() const;

                bool isConnected(RouterHash const &rh) const;

                /**
                 * Stops the transport. That is, iterates over all connected peers and sends
                 *  them a session destroyed i2pcpp::Destroyed. Then stops the IO service
                 *  and joins all of the service threads.
                 */
                void shutdown();

            private:
                std::unique_ptr<Context> m_impl;
        };
    }
}

#endif
