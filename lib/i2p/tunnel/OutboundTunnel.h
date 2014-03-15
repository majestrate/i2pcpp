#ifndef TUNNELOUTBOUNDTUNNEL_H
#define TUNNELOUTBOUNDTUNNEL_H

#include "Tunnel.h"

#include <vector>

namespace i2pcpp {
    namespace Tunnel {
        class OutboundTunnel : public Tunnel {
            public:
                /**
                 * Constructs an outbound tunnel given a vector of RouterIdentities \a hops, a \a replyHash, and a \a replyTunnelId.
                 */
                OutboundTunnel(std::vector<RouterIdentity> const &hops, RouterHash const &replyHash, uint32_t const replyTunnelId);

                /**
                 * Returns the direction of this tunnel (always outbound).
                 */
                Tunnel::Direction getDirection() const;
        };
    }
}

#endif
