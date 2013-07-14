#ifndef TUNNEL_H
#define TUNNEL_H

#include <list>

#include "../datatypes/BuildRequestRecord.h"

#include "TunnelHop.h"

namespace i2pcpp {
	class Tunnel {
		public:
			enum Direction {
				INBOUND,
				OUTBOUND
			};

			enum State {
				REQUESTED,
				OPERATIONAL,
				FAILED
			};

			Tunnel(Direction d, std::vector<RouterIdentity> const &hops, RouterHash const &myHash);

			State getState() const;
			std::list<BuildRecordPtr> getRecords() const;
			RouterHash getDownstream() const;

		private:
			std::list<TunnelHopPtr> m_hops;
			Direction m_direction;
			State m_state = REQUESTED;
	};

	typedef std::shared_ptr<Tunnel> TunnelPtr;
}

#endif
