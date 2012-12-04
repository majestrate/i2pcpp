#ifndef SSUPACKETPARSER_H
#define SSUPACKETPARSER_H

#include "Packet.h"

#include "InboundEstablishmentState.h"
#include "OutboundEstablishmentState.h"

namespace i2pcpp {
	namespace SSU {
		class PacketParser {
			public:
				void parsePacketInbound(PacketPtr const &p, InboundEstablishmentStatePtr const &state);
				void parsePacketOutbound(PacketPtr const &p, OutboundEstablishmentStatePtr const &state);

			private:
				void parseSessionCreated(PacketPtr const &p, OutboundEstablishmentStatePtr const &state);
		};
	}
}

#endif
