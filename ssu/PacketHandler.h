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
				PacketHandler(UDPTransport &transport);

				void packetReceived(PacketPtr &p);

			private:
				void handlePacket(PacketPtr const &packet, PeerStatePtr const &state);
				void handlePacket(PacketPtr const &packet, EstablishmentStatePtr const &state);
				void handleNewPacket(PacketPtr const &packet);
				void handleSessionRequest(ByteArray::const_iterator &dataItr, EstablishmentStatePtr const &state);
				void handleSessionCreated(ByteArray::const_iterator &dataItr, EstablishmentStatePtr const &state);
				void handleSessionConfirmed(ByteArray::const_iterator &dataItr, ByteArray::const_iterator end, EstablishmentStatePtr const &state);

				UDPTransport& m_transport;

				SessionKey m_inboundKey;
				InboundMessageFragments m_imf;
		};
	}
}

#endif
