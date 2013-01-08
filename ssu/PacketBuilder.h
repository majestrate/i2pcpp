#ifndef SSUPACKETBUILDER_H
#define SSUPACKETBUILDER_H

#include <forward_list>

#include "Packet.h"
#include "EstablishmentState.h"
#include "PeerState.h"
#include "OutboundMessageState.h"

#include "../datatypes/Endpoint.h"

namespace i2pcpp {
	namespace SSU {
		class PacketBuilder {
			public:
				static PacketPtr buildSessionRequest(EstablishmentStatePtr const &state);
				static PacketPtr buildSessionConfirmed(EstablishmentStatePtr const &state);
				static PacketPtr buildData(PeerStatePtr const &ps, bool wantReply, std::forward_list<OutboundMessageState::FragmentPtr> const &fragments, AckList const &acks);
				static PacketPtr buildSessionDestroyed(PeerStatePtr const &state);

			private:
				static PacketPtr buildHeader(Endpoint const &ep, unsigned char flag);
		};
	}
}

#endif
