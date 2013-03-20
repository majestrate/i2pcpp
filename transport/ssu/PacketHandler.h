#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include "Packet.h"
#include "PeerState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport);

				void packetReceived(PacketPtr &p, PeerStatePtr &ps);

			private:
				void handleNewPacket(PacketPtr &p);

				UDPTransport& m_transport;
		};
	}
}

#endif
