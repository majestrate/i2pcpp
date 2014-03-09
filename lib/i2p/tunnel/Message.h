#ifndef MESSAGE_H
#define MESSAGE_H

#include "Fragment.h"

#include "../i2np/Message.h"

#include <i2pcpp/datatypes/ByteArray.h>
#include <i2pcpp/datatypes/StaticByteArray.h>

#include <botan/symkey.h>

#include <array>
#include <list>

namespace i2pcpp {
    class Message {
        public:
            Message(StaticByteArray<1024> const &data);
            Message(std::list<FragmentPtr> &fragments);

            std::list<FragmentPtr> parse() const;
            StaticByteArray<1024> getEncryptedData() const;
            void encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey);
            void compile();

        private:
            void calculateChecksum();
            bool verifyChecksum() const;

            uint32_t m_checksum;
            std::list<FragmentPtr> m_fragments;
            uint16_t m_payloadSize = 0;

            StaticByteArray<16> m_iv;
            StaticByteArray<1008> m_encrypted;
    };
}

#endif
