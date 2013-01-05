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

				PacketPtr buildSessionRequest(EstablishmentStatePtr const &state) const;
				PacketPtr buildSessionConfirmed(EstablishmentStatePtr const &state) const;
				PacketPtr buildData(PeerStatePtr const &ps, bool wantReply, std::forward_list<OutboundMessageState::FragmentPtr> const &fragments) const;
				PacketPtr buildSessionDestroyed(PeerStatePtr const &state) const;

			private:
				PacketPtr buildHeader(Endpoint const &ep, unsigned char flag) const;
		};
	}
}

#endif
