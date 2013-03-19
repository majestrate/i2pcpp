#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include "Packet.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class PacketHandler {
			public:
				void packetReceived(PacketPtr &p, PeerStatePtr &ps);

			private:
		};
	}
}

#endif
