#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include <atomic>
#include <thread>
#include <memory>

#include "OutboundEstablishmentState.h"
#include "Packet.h"
#include "PeerState.h"
#include "InboundMessageFragments.h"

#include "../Thread.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class PacketHandler : public Thread {
			public:
				PacketHandler(UDPTransport &transport) : m_transport(transport), m_imf(transport) {}

			private:
				void startHook() { m_imf.begin(); }
				void loop();
				void stopHook() { m_imf.shutdown(); }

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
