#ifndef TUNNELMESSAGE_H
#define TUNNELMESSAGE_H

#include "Fragment.h"

#include "../i2np/Message.h"

#include <i2pcpp/datatypes/ByteArray.h>
#include <i2pcpp/datatypes/StaticByteArray.h>

#include <botan/symkey.h>

#include <array>
#include <list>

namespace i2pcpp {
    namespace Tunnel {
        class Message {
            public:
                /**
                 * Constructs a Message from 16 bytes of IV and 1008 bytes
                 * of encrypted data.
                 */
                Message(StaticByteArray<1024> const &data);

                /**
                 * Constructs a Message from a list of unencrpyted fragments.
                 */
                Message(std::list<FragmentPtr> &fragments);

                /**
                 * After the Message has been decrypted, this method will
                 * parse the data.
                 * @return a list of i2pcpp::Tunnel::FragmentPtr
                 */
                std::list<FragmentPtr> parse() const;

                /**
                 * @return the encrypted message, if it was set previously.
                 */
                StaticByteArray<1024> getEncryptedData() const;

                /**
                 * Encrypts the compiled message.
                 */
                void encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey);

                /**
                 * Compiles the fragments together in preparation for
                 * encryption.
                 */
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
}

#endif
