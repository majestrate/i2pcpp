#include "Tunnel.h"

#include <botan/auto_rng.h>

#include "../Database.h"
#include "../datatypes/RouterInfo.h"
#include "../i2np/VariableTunnelBuild.h"
#include "../OutboundMessageDispatcher.h"

#include <iomanip>

namespace i2pcpp {
	Tunnel::Tunnel(RouterContext &ctx, std::list<RouterHash> const &hops) : m_ctx(ctx)
	{
		Botan::AutoSeeded_RNG rng;

		for(auto h = hops.cbegin(); h != hops.cend(); ++h) {
			const RouterInfo&& ri = m_ctx.getDatabase().getRouterInfo(*h);

			uint32_t tunnelId, nextTunnelId;
			rng.randomize((unsigned char *)&tunnelId, sizeof(tunnelId));
			rng.randomize((unsigned char *)&nextTunnelId, sizeof(nextTunnelId));

			SessionKey tunnelLayerKey, tunnelIVKey, replyKey, replyIV;
			rng.randomize(tunnelLayerKey.data(), tunnelLayerKey.size());
			rng.randomize(tunnelIVKey.data(), tunnelIVKey.size());
			rng.randomize(replyKey.data(), replyKey.size());
			rng.randomize(replyIV.data(), replyIV.size());

			RouterHash nextIdentity;
			if(h == --hops.cend())
				nextIdentity = m_ctx.getMyRouterIdentity().getHash();
			else
				nextIdentity = *next(h);

			BuildRequestRecord::HopType htype = BuildRequestRecord::HopType::OUTBOUND_EP;

			uint32_t hoursSinceEpoch = std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();

			uint32_t nextMsgId;
			rng.randomize((unsigned char *)&nextMsgId, sizeof(nextMsgId));

			m_records.emplace_back(tunnelId, *h, nextTunnelId, nextIdentity, tunnelLayerKey, tunnelIVKey, replyKey, replyIV, htype, hoursSinceEpoch, nextMsgId);
			m_records.back().encrypt(ri.getIdentity().getEncryptionKey());
			std::cerr << "finished Tunnel ctor\n";
		}
	}

	void Tunnel::build()
	{
		std::cerr << "starting build\n";
		for(auto h: m_records) {
			I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(m_records));
			m_ctx.getOutMsgDispatcher().sendMessage(h.getLocalIdentity(), vtb);
			std::cerr << "sent record\n";
		}
	}
}
