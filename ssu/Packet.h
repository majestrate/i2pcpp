#ifndef SSUPACKET_H
#define SSUPACKET_H

#include <memory>

#include <botan/lookup.h>

#include "../datatypes/Endpoint.h"
#include "../datatypes/ByteArray.h"
#include "../datatypes/SessionKey.h"

namespace i2pcpp {
	namespace SSU {
		class Packet {
			public:
				Packet() : m_endpoint(Endpoint("", 0)) {}
				Packet(Endpoint const &endpoint) : m_endpoint(endpoint) {}
				Packet(Endpoint const &endpoint, const unsigned char *data, size_t length);

				void decrypt(SessionKey const &mk);
				bool verify(SessionKey const &sk);
				void encrypt(SessionKey const &sk, SessionKey const &mk);
				void encrypt(Botan::InitializationVector const &iv, SessionKey const &sk, SessionKey const &mk);

				ByteArray& getData() { return m_data; }
				Endpoint getEndpoint() { return m_endpoint; }

				enum PayloadType {
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

				static const unsigned short MIN_PACKET_LEN = 48;

			private:
				ByteArray m_data;
				Endpoint m_endpoint;

				static const unsigned short PROTOCOL_VERSION = 0;
		};

		typedef std::shared_ptr<Packet> PacketPtr;
	}
}

#endif
