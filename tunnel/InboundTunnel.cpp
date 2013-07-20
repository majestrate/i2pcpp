#include "InboundTunnel.h"

namespace i2pcpp {
	InboundTunnel::InboundTunnel(std::vector<RouterIdentity> const &hops)
	{
		uint32_t lastTunnelId;
		RouterHash lastRouterHash;
		TunnelHopPtr h;

		if(hops.size() < 2)
			throw std::logic_error("Bad input to InboundTunnel ctor");

		for(int i = 0; i < hops.size(); i++) {
			if(!i) {
				h = std::make_shared<TunnelHop>(hops[i], RouterHash());
				m_tunnelId = h->getNextTunnelId();
			} else
				h = std::make_shared<TunnelHop>(hops[i], lastRouterHash, lastTunnelId);

			lastTunnelId = h->getTunnelId();
			lastRouterHash = h->getLocalHash();

			m_hops.push_front(h);
		}

		m_gateway = m_hops.front()->getTunnelId();
		m_hops.front()->setType(TunnelHop::GATEWAY);
	}

	Tunnel::Direction InboundTunnel::getDirection() const
	{
		return Direction::INBOUND;
	}
}
