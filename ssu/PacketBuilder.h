#ifndef SSUPACKETBUILDER_H
#define SSUPACKETBUILDER_H

#include "Packet.h"
#include "EstablishmentState.h"
#include "PeerState.h"

#include "../datatypes/Endpoint.h"

namespace i2pcpp {
	namespace SSU {
		class PacketBuilder {
			public:
				PacketPtr buildSessionRequest(EstablishmentStatePtr const &state) const;
				PacketPtr buildSessionConfirmed(EstablishmentStatePtr const &state) const;
				PacketPtr buildSessionDestroyed(PeerStatePtr const &state) const;

			private:
				PacketPtr buildHeader(Endpoint const &ep, unsigned char flag) const;
		};
	}
}

#endif
