/**
 * @file Packet.cpp
 * @brief Implements Packet.h
 */
#include "Packet.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/md5.h>

#include <i2pcpp/util/I2PHMAC.h>
#include <i2pcpp/util/Base64.h>

namespace i2pcpp {
    namespace SSU {
        Packet::Packet(Endpoint const &endpoint) :
            m_endpoint(endpoint) {}

        Packet::Packet(Endpoint const &endpoint, const unsigned char *data, size_t length) :
            m_endpoint(endpoint)
        {
            m_data.resize(length);
            std::copy(data, data + length, m_data.begin());
        }

        void Packet::decrypt(SessionKey const &sk)
        {
            const unsigned char *packetIV = m_data.data() + 16;
            const unsigned char *packet = m_data.data() + 32;
            const unsigned int packetSize = ((m_data.size() - 32) / 16) * 16;

            Botan::InitializationVector iv(packetIV, 16);
            Botan::SymmetricKey key(sk.data(), sk.size());
            Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", key, iv, Botan::DECRYPTION));

            cipherPipe.process_msg(packet, packetSize);

            size_t decryptedSize = cipherPipe.remaining();
            ByteArray plaintext(decryptedSize);

            cipherPipe.read(plaintext.data(), decryptedSize);
            m_data = plaintext;
        }

        bool Packet::verify(SessionKey const &mk)
        {
            unsigned int packetSize = m_data.size() - 32;

            Botan::SymmetricKey key(mk.data(), mk.size());
            Botan::Pipe hmacPipe(new Botan::MAC_Filter(new I2PHMAC(new Botan::MD5()), key));

            hmacPipe.start_msg();
            hmacPipe.write(m_data.data() + 32, packetSize);
            hmacPipe.write(m_data.data() + 16, 16);
            hmacPipe.write((packetSize >> 8) ^ (PROTOCOL_VERSION >> 8));
            hmacPipe.write(packetSize ^ (PROTOCOL_VERSION));
            hmacPipe.end_msg();

            ByteArray calculatedMAC(16);
            hmacPipe.read(calculatedMAC.data(), 16);

            return calculatedMAC == ByteArray(m_data.begin(), m_data.begin() + 16);
        }

        void Packet::encrypt(SessionKey const &sk, SessionKey const &mk)
        {
            Botan::AutoSeeded_RNG rng;
            Botan::InitializationVector iv(rng, 16);

            encrypt(iv, sk, mk);
        }

        void Packet::encrypt(Botan::InitializationVector const &iv, SessionKey const &sk, SessionKey const &mk)
        {
            Botan::SymmetricKey sessionKey(sk.data(), sk.size());
            Botan::SymmetricKey macKey(mk.data(), mk.size());
            Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", sessionKey, iv, Botan::ENCRYPTION));
            Botan::Pipe hmacPipe(new Botan::MAC_Filter(new I2PHMAC(new Botan::MD5()), macKey));

            unsigned char padSize = 16 - (m_data.size() % 16);
            if(padSize < 16)
                m_data.insert(m_data.end(), padSize, padSize);

            cipherPipe.process_msg(m_data.data(), m_data.size());

            size_t encryptedSize = cipherPipe.remaining();
            m_data.resize(encryptedSize + 32);

            cipherPipe.read(m_data.data() + 32, encryptedSize);
            if(cipherPipe.remaining())
                throw std::runtime_error("Bytes still remaining in the cipherPipe!?"); // TODO For real

            copy(iv.begin(), iv.end(), m_data.begin() + 16);

            hmacPipe.start_msg();
            hmacPipe.write(m_data.data() + 32, encryptedSize);
            hmacPipe.write(iv.bits_of());
            hmacPipe.write((encryptedSize >> 8) ^ (PROTOCOL_VERSION >> 8));
            hmacPipe.write(encryptedSize ^ (PROTOCOL_VERSION));
            hmacPipe.end_msg();

            hmacPipe.read(m_data.data(), 16);
            if(hmacPipe.remaining())
                throw std::runtime_error("Bytes still remaining in the hmacPipe!?"); // TODO For real
        }

        ByteArray& Packet::getData()
        {
            return m_data;
        }

        Endpoint Packet::getEndpoint() const
        {
            return m_endpoint;
        }
    }
}
