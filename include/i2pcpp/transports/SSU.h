/**
 * @file SSU.h
 * @brief Defines the i2pcpp::SSU::SSU class.
 */
#ifndef SSU_H
#define SSU_H

#include <i2pcpp/Transport.h>

#include <i2pcpp/datatypes/RouterIdentity.h>

namespace i2pcpp {
    namespace SSU {
        class SSU : public Transport {
            public:
                /**
                 * Constructs an SSU transport given a private DSA key and
                 * RouterIdentity. The DSA key is used for signing and the
                 * RouterIdentity key (ElGamal) is used for encryption.
                 */
                SSU(std::string const &privKeyPEM, RouterIdentity const &ri);

            private:
                struct SSUImpl;
                std::unique_ptr<SSUImpl> m_impl;
        };
    }
}

#endif
