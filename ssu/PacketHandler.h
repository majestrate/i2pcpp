#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include <thread>
#include <memory>

#include "OutboundEstablishmentState.h"
#include "Packet.h"
#include "PeerState.h"
#include "InboundMessageFragments.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport) : m_transport(transport), m_imf(transport) {}

				void run();

			private:
				void handlePacket(PacketPtr const &packet,	PeerStatePtr const &state);
				void handlePacket(PacketPtr const &packet, OutboundEstablishmentStatePtr const &state);
				void handleSessionCreated(ByteArray::const_iterator &dataItr, OutboundEstablishmentStatePtr const &state);
				void startMessageReceiver();

				UDPTransport &m_transport;
				InboundMessageFragments m_imf;
		};
	}
}

#endif
