#ifndef TUNNELINBOUNDTUNNEL_H
#define TUNNELINBOUNDTUNNEL_H

#include "Tunnel.h"

#include <vector>

namespace i2pcpp {
    namespace Tunnel {
        class InboundTunnel : public Tunnel {
            public:
                /**
                 * Constructs an inbound tunnel given the current router
                 * hash \a myHash and the router identities of the hops
                 * for the tunnel.
                 */
                InboundTunnel(RouterHash const &myHash, std::vector<RouterIdentity> const &hops = {});

                /**
                 * Returns the direction of this tunnel (always inbound).
                 */
                Tunnel::Direction getDirection() const;
        };
    }
}

#endif
