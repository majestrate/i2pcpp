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

			virtual Direction getDirection() const = 0;

			State getState() const;
			uint32_t getTunnelId() const;
			std::list<BuildRecordPtr> getRecords() const;
			RouterHash getDownstream() const;

		protected:
			std::list<TunnelHopPtr> m_hops;
			State m_state = REQUESTED;
			uint32_t m_tunnelId;
			uint32_t m_gateway;
	};

	typedef std::shared_ptr<Tunnel> TunnelPtr;
}

#endif
