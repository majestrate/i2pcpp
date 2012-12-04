#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include <memory>

#include "OutboundEstablishmentState.h"
#include "Packet.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/ByteArray.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport) : m_transport(transport) {}

				void run();

			private:
				void handleSessionCreated(ByteArray::iterator dataItr, OutboundEstablishmentStatePtr const &state);

				UDPTransport &m_transport;
		};
	}
}

#endif
