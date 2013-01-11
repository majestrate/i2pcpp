#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include "Packet.h"
#include "PeerStateList.h"
#include "EstablishmentManager.h"
#include "InboundMessageFragments.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport) : m_transport(transport), m_imf(transport) {}

				void packetReceived(PacketPtr &p);

			private:
				void handlePacket(PacketPtr const &packet, PeerStatePtr const &state);
				void handlePacketOutbound(PacketPtr const &packet, EstablishmentStatePtr const &state);
				void handleSessionCreated(ByteArray::const_iterator &dataItr, EstablishmentStatePtr const &state);

				UDPTransport& m_transport;

				InboundMessageFragments m_imf;
		};
	}
}

#endif
