#ifndef INBOUNDTUNNEL_H
#define INBOUNDTUNNEL_H

#include <vector>

#include "Tunnel.h"

namespace i2pcpp {
	class InboundTunnel : public Tunnel {
		public:
			InboundTunnel(RouterHash const &myHash, std::vector<RouterIdentity> const &hops = {});

			Tunnel::Direction getDirection() const;

			SessionKey getTunnelLayerKey() const;
			SessionKey getTunnelIVKey() const;

		private:
			SessionKey m_tunnelLayerKey;
			SessionKey m_tunnelIVKey;
	};
}

#endif
