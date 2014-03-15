#include "InboundTunnel.h"

#include <botan/auto_rng.h>

#include <i2pcpp/datatypes/RouterIdentity.h>

namespace i2pcpp {
    namespace Tunnel {
        InboundTunnel::InboundTunnel(RouterHash const &myHash, std::vector<RouterIdentity> const &hops)
        {
            /* Zero hop tunnel */
            if(hops.empty()) {
                Botan::AutoSeeded_RNG rng;
                rng.randomize((unsigned char *)&m_tunnelId, sizeof(m_tunnelId));
                m_state = State::OPERATIONAL;
                return;
            }

            uint32_t lastTunnelId;
            RouterHash lastRouterHash;
            BuildRequestRecordPtr h;

            for(int i = 0; i < hops.size(); i++) {
                if(!i) {
                    h = std::make_shared<BuildRequestRecord>(hops[i], myHash);
                    m_tunnelId = h->getNextTunnelId();
                    m_nextMsgId = h->getNextMsgId();
                } else
                    h = std::make_shared<BuildRequestRecord>(hops[i], lastRouterHash, lastTunnelId);

                lastTunnelId = h->getTunnelId();
                lastRouterHash = h->getLocalHash();

                m_hops.push_front(h);
            }

            std::static_pointer_cast<BuildRequestRecord>(m_hops.front())->setType(BuildRequestRecord::Type::GATEWAY);

            secureRecords();
        }

        Tunnel::Direction InboundTunnel::getDirection() const
        {
            return Direction::INBOUND;
        }
    }
}
