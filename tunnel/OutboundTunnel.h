#ifndef OUTBOUNDTUNNEL_H
#define OUTBOUNDTUNNEL_H

#include <vector>

#include "Tunnel.h"

namespace i2pcpp {
	class OutboundTunnel : public Tunnel {
		public:
			OutboundTunnel(std::vector<RouterIdentity> const &hops, RouterHash const &replyHash, uint32_t const replyTunnelId);

			Tunnel::Direction getDirection() const;
	};
}

#endif
