#include "Tunnel.h"

#include <botan/auto_rng.h>

#include "../Database.h"
#include "../datatypes/RouterInfo.h"

namespace i2pcpp {
	Tunnel::Tunnel(RouterContext &ctx, std::list<RouterHash> const &hops) : m_ctx(ctx)
	{
		Botan::AutoSeeded_RNG rng;

		for(auto h = hops.cbegin(); h != hops.cend(); ++h) {
			const RouterInfo&& ri = m_ctx.getDatabase().getRouterInfo(*h);

			uint32_t tunnelId;
			rng.randomize((unsigned char *)&tunnelId, sizeof(tunnelId));
			SessionKey tunnelLayerKey, tunnelIVKey, replyKey, replyIV;
			rng.randomize(tunnelLayerKey.data(), tunnelLayerKey.size());
			rng.randomize(tunnelIVKey.data(), tunnelIVKey.size());
			rng.randomize(replyKey.data(), replyKey.size());
			rng.randomize(replyIV.data(), replyIV.size());
			//BuildRequestRecord brr(tunnelId, h, h+1, );
		}
	}
}
