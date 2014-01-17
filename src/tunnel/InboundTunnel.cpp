#include "InboundTunnel.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
    InboundTunnel::InboundTunnel(RouterHash const &myHash, std::vector<RouterIdentity> const &hops)
    {
        uint32_t lastTunnelId;
        RouterHash lastRouterHash;
        TunnelHopPtr h;

        if(hops.empty()) {
            Botan::AutoSeeded_RNG rng;
            rng.randomize((unsigned char *)&m_tunnelId, sizeof(m_tunnelId));
            m_state = State::OPERATIONAL;
            return;
        }

        for(int i = 0; i < hops.size(); i++) {
            if(!i) {
                h = std::make_shared<TunnelHop>(hops[i], myHash);
                m_tunnelId = h->getNextTunnelId();
                m_nextMsgId = h->getNextMsgId();
            } else
                h = std::make_shared<TunnelHop>(hops[i], lastRouterHash, lastTunnelId);

            lastTunnelId = h->getTunnelId();
            lastRouterHash = h->getLocalHash();

            m_hops.push_front(h);
        }

        m_hops.front()->setType(TunnelHop::Type::GATEWAY);
    }

    Tunnel::Direction InboundTunnel::getDirection() const
    {
        return Direction::INBOUND;
    }
}
