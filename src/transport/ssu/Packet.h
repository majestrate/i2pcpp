/**
 * @file Packet.h
 * @brief Defines the i2pcpp::SSU::Packet structure.
 */
#ifndef SSUPACKET_H
#define SSUPACKET_H

#include <memory>

#include <botan/lookup.h>

#include <i2pcpp/datatypes/Endpoint.h>
#include <i2pcpp/datatypes/ByteArray.h>
#include <i2pcpp/datatypes/SessionKey.h>

namespace i2pcpp {
    namespace SSU {
        /**
         * Represents an SSU packet and provides cryptography functionality.
         */
        class Packet {
            public:

                /**
                 * Constructs from a given, remote i2pcpp::Endpoint.
                 * @param endpoint remote endpoint associated with packet
                 */
                Packet(Endpoint const &endpoint);

                /**
                 * Constructs given a remote i2pcpp::Endpoint and data.
                 * @param endpoint remote endpoint associated with packet
                 * @param data the received data
                 * @param length the length of \a data in bytes
                 */
                Packet(Endpoint const &endpoint, const unsigned char *data, size_t length);

                /**
                 * Decrypts this packet given an i2pcpp::SessionKey.
                 * The algorithm used is AES-256 (CBC mode, no padding).
                 */
                void decrypt(SessionKey const &sk);

                /**
                 * Verifies the (H)MAC of the current packet.
                 * The i2pcpp::SessionKey is the required HMAC key.
                 * The hash algorithm used is MD5.
                 */
                bool verify(SessionKey const &mk);

                /**
                 * Encrypts this packet given a key for AES \a sk and a HMAC key \a mk.
                 * The algorithm for the former is AES-256 (CBC mode, no padding).
                 * The hash algorithm for the latter is MD5.
                 * The IV is randomly generated.
                 */
                void encrypt(SessionKey const &sk, SessionKey const &mk);

                /**
                 * Encrypts this packet given a key for AES \a sk and a HMAC key \mk.
                 * The algorithm for the former is AES-256 (CBC mode, no padding).
                 * @param iv the IV to use for CBC mode
                 * @throw std::runtime_error
                 */
                void encrypt(Botan::InitializationVector const &iv, SessionKey const &sk, SessionKey const &mk);

                /**
                 * @return the packet data as an i2pcpp::ByteArray
                 */
                ByteArray& getData();

                /**
                 * @return the associated remote i2pcpp::Endpoint
                 */
                Endpoint getEndpoint() const;

                /**
                 * Defines the possible packet types for SSU.
                 */
                enum class PayloadType {
                    SESSION_REQUEST = 0,
                    SESSION_CREATED = 1,
                    SESSION_CONFIRMED = 2,
                    RELAY_REQUEST = 3,
                    RELAY_RESPONSE = 4,
                    RELAY_INTRO = 5,
                    DATA = 6,
                    TEST = 7,
                    SESSION_DESTROY = 8
                };

                /// Minimum packet length
                static const unsigned short MIN_PACKET_LEN = 48;

            private:
                ByteArray m_data;
                Endpoint m_endpoint;

                /// Version of the SSU protcol
                static const unsigned short PROTOCOL_VERSION = 0;
        };

        /**
         * Utility typedef so that i2pcpp::SSU::PacketPtr is an
         *  i2pcpp::SSU::Packet wrapped in a std::shared_ptr.
         */
        typedef std::shared_ptr<Packet> PacketPtr;
    }
}

#endif
