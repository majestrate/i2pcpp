/**
 * @file BuildRecord.h
 * @brief Contains the definition of the i2pcpp::BuildRecord type.
 */
#ifndef BUILDRECORD_H
#define BUILDRECORD_H

#include "Datatype.h"
#include "ByteArray.h"
#include "SessionKey.h"

#include <bitset>
#include <memory>

namespace Botan { class ElGamal_PrivateKey; }

namespace i2pcpp {
    /**
     * Base class for tunnel request/reply build record. Implements cryptography operations.
     */
    class BuildRecord : public Datatype {
        public:
            BuildRecord() = default;

            /**
             * Constructs an i2pcpp::BuildRecord from a ByteArray, given by an iterator
             *  to its begin and end.
             */
            BuildRecord(ByteArrayConstItr &begin, ByteArrayConstItr end);

            virtual ~BuildRecord() {}

            /**
             * Serializes the i2pcpp::BuildRecord object.
             * @return the serialized object as an i2pcpp::ByteArray.
             */
            ByteArray serialize() const;

            /**
             * Preforms ElGamal encryption on the build record data type.
             * @param encryptionKey the public key to be used for ElGamal decryption
             */
            void encrypt(ByteArray const &encryptionKey);

            /**
             * Preforms ElGamal decryption on the build record data type.
             * @param key the private key to be used for ElGamal decryption
             */
            void decrypt(std::shared_ptr<const Botan::ElGamal_PrivateKey> key);

            /**
             * Preforms AES encryption on the build record data.
             * @param iv the 16 byte initialization vector for AES
             * @param key the i2pcpp::SessionKey to be used
             */
            void encrypt(StaticByteArray<16> const &iv, SessionKey const &key);

            /**
             * Preforms AES decryption on the build record data.
             * @param iv the 16 byte initialization vector for AES
             * @param key the i2pcpp::SessionKey to be used
             */
            void decrypt(StaticByteArray<16> const &iv, SessionKey const &key);

            /**
             * Sets the header of the encrypted message, that is the first
             *  16 bytes of the router hash.
             */
            void setHeader(StaticByteArray<16> const &header);

            /**
             * Gets the header or the first 16 bytes of the router hash.
             */
            const StaticByteArray<16>& getHeader() const;

        protected:
            /// The first 16 bytes of the router hash
            StaticByteArray<16> m_header;
            StaticByteArray<512> m_data;
    };

    typedef std::shared_ptr<BuildRecord> BuildRecordPtr;
}

#endif
