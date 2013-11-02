#include "OutboundTunnel.h"

namespace i2pcpp {
	OutboundTunnel::OutboundTunnel(std::vector<RouterIdentity> const &hops, RouterHash const &replyHash, uint32_t const replyTunnelId)
	{
		uint32_t lastTunnelId;
		RouterHash lastRouterHash;

		for(int i = hops.size() - 1; i >= 0; i--) {
			TunnelHopPtr h;

			if(i == hops.size() - 1) {
				h = std::make_shared<TunnelHop>(hops[i], replyHash);
				h->setType(TunnelHop::ENDPOINT);
				h->setNextTunnelId(replyTunnelId);
				m_tunnelId = h->getNextTunnelId();
				m_nextMsgId = h->getNextMsgId();
			} else
				h = std::make_shared<TunnelHop>(hops[i], lastRouterHash, lastTunnelId);

			lastTunnelId = h->getTunnelId();
			lastRouterHash = h->getLocalHash();

			m_hops.push_front(h);
		}
	}

	Tunnel::Direction OutboundTunnel::getDirection() const
	{
		return Direction::OUTBOUND;
	}
}
