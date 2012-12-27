#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include <thread>
#include <memory>

#include "OutboundEstablishmentState.h"
#include "Packet.h"
#include "PeerState.h"
#include "InboundMessageFragments.h"
#include "MessageReceiver.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/ByteArray.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport) : m_transport(transport), m_messageReceiver(*this), m_keepRunning(true) {}

				void run();
				bool keepRunning() const { return m_keepRunning; }

			private:
				void handlePacket(PacketPtr const &packet,	PeerStatePtr const &state);
				void handlePacket(PacketPtr const &packet, OutboundEstablishmentStatePtr const &state);
				void handleSessionCreated(ByteArray::const_iterator &dataItr, OutboundEstablishmentStatePtr const &state);
				void startMessageReceiver();

				UDPTransport &m_transport;
				InboundMessageFragments m_imf;
				MessageReceiver m_messageReceiver;
				thread m_messageReceiverThread;
				bool m_keepRunning;
		};
	}
}

#endif
