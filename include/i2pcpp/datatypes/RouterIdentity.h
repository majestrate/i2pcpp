/**
 * @file RouterIdentity.h
 * @brief Defines the i2pcpp::RouterIdentity data structure.
 */
#ifndef ROUTERIDENTITY_H
#define ROUTERIDENTITY_H

#include <array>

#include "Datatype.h"
#include "RouterHash.h"
#include "Certificate.h"

namespace i2pcpp {
    /**
     * Allows a router to be uniquely identified.
     */
    class RouterIdentity : public Datatype {
        public:
            /**
             * Constructs from iterators to the begin and end of an i2pcpp::ByteArray.
             * Format must be as follows:
             * public key (256B) | signing key (128B) | cerificate (>= 3B)
             * @throw i2pcpp::FormattingError
             */
            RouterIdentity(ByteArrayConstItr &begin, ByteArrayConstItr end);

            /**
             * @param public key for ElGamal encryption
             * @param signing public key for verifying DSA certificates
             */
            RouterIdentity(ByteArray const &encryptionKey, ByteArray const &signingKey, Certificate const &certificate);

            /**
             * Serialies the object as follows:
             * public key (256B) | signing key (128B) | cerificate (>= 3B)
             */
            ByteArray serialize() const;

            /**
             * @return the public encryption key
             */
            ByteArray getEncryptionKey() const;

            /**
             * @return the private encryption key
             */
            ByteArray getSigningKey() const;

            /**
             * @return SHA256 hash of the i2pcpp::RouterIdentity object
             */
            RouterHash getHash() const;

            /**
             * @return router identity certificate
             */
            const Certificate& getCertificate() const;

        private:
            std::array<unsigned char, 256> m_encryptionKey;
            std::array<unsigned char, 128> m_signingKey;
            Certificate m_certificate;

            mutable bool m_hashed = false;
            mutable RouterHash m_hash;
    };
}

#endif
