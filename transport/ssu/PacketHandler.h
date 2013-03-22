#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include "Packet.h"
#include "PeerState.h"
#include "EstablishmentState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport, SessionKey const &sk);

				void packetReceived(PacketPtr &p, PeerStatePtr &ps);

			private:
				void handleNewPacket(PacketPtr &p);

				void handleSessionRequest(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state);

				UDPTransport& m_transport;

				SessionKey m_inboundKey;
		};
	}
}

#endif
