#include "OutboundTunnel.h"

#include <i2pcpp/datatypes/RouterIdentity.h>

namespace i2pcpp {
    namespace Tunnel {
        OutboundTunnel::OutboundTunnel(std::vector<RouterIdentity> const &hops, RouterHash const &replyHash, uint32_t const replyTunnelId)
        {
            /* Zero hop tunnel */
            if(hops.empty()) {
                m_state = State::OPERATIONAL;
                return;
            }

            uint32_t lastTunnelId;
            RouterHash lastRouterHash;

            for(int i = hops.size() - 1; i >= 0; i--) {
                BuildRequestRecordPtr h;

                if(i == hops.size() - 1) {
                    h = std::make_shared<BuildRequestRecord>(hops[i], replyHash);
                    h->setType(BuildRequestRecord::Type::ENDPOINT);
                    h->setNextTunnelId(replyTunnelId);
                    m_tunnelId = h->getNextTunnelId();
                    m_nextMsgId = h->getNextMsgId();
                } else {
                    h = std::make_shared<BuildRequestRecord>(hops[i], lastRouterHash, lastTunnelId);
                }
                lastTunnelId = h->getTunnelId();
                lastRouterHash = h->getLocalHash();

                m_hops.push_front(h);
            }

            secureRecords();
        }

        Tunnel::Direction OutboundTunnel::getDirection() const
        {
            return Direction::OUTBOUND;
        }
    }
}
