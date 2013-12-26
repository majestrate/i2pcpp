#ifndef BUILDRECORD_H
#define BUILDRECORD_H

#include <bitset>
#include <memory>

#include <botan/elgamal.h>

#include "Datatype.h"
#include "ByteArray.h"
#include "SessionKey.h"

namespace i2pcpp {
    class BuildRecord : public Datatype {
        public:
            BuildRecord() = default;
            BuildRecord(ByteArrayConstItr &begin, ByteArrayConstItr end);
            virtual ~BuildRecord() {}

            ByteArray serialize() const;

            void encrypt(ByteArray const &encryptionKey);
            void decrypt(std::shared_ptr<const Botan::ElGamal_PrivateKey> key);
            void encrypt(StaticByteArray<16> const &iv, SessionKey const &key);
            void decrypt(StaticByteArray<16>  const &iv, SessionKey const &key);

            void setHeader(StaticByteArray<16> const &header);
            const StaticByteArray<16>& getHeader() const;

        protected:
            StaticByteArray<16> m_header;
            StaticByteArray<512> m_data;
    };

    typedef std::shared_ptr<BuildRecord> BuildRecordPtr;
}

#endif
