#ifndef TUNNEL_H
#define TUNNEL_H

#include <list>

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

			Tunnel(Direction d);

			State getState() const;

		private:
			std::list<TunnelHopPtr> m_hops;
			Direction m_direction;
			State m_state = REQUESTED;
	};

	typedef std::shared_ptr<Tunnel> TunnelPtr;
}

#endif
