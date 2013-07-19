#ifndef INBOUNDTUNNEL_H
#define INBOUNDTUNNEL_H

#include <vector>

#include "Tunnel.h"

namespace i2pcpp {
	class InboundTunnel : public Tunnel {
		public:
			InboundTunnel(std::vector<RouterIdentity> const &hops);

			Tunnel::Direction getDirection() const;
	};
}

#endif
