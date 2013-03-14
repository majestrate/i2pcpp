#include "InboundTunnel.h"

#include <botan/auto_rng.h>

#include "../Database.h"
#include "../datatypes/RouterInfo.h"
#include "../i2np/VariableTunnelBuild.h"
#include "../OutboundMessageDispatcher.h"

#include "../util/Base64.h"

#include <iomanip>

namespace i2pcpp {
	InboundTunnel::InboundTunnel(RouterContext &ctx, std::list<RouterHash> &hops) : m_ctx(ctx)
	{
		Botan::AutoSeeded_RNG rng;

		std::vector<uint32_t> tunnelIds;
		std::vector<SessionKey> tunnelLayerKeys;
		std::vector<SessionKey> tunnelIVKeys;
		std::vector<SessionKey> replyKeys;
		std::vector<SessionKey> replyIVs;
		std::vector<uint32_t> nextMsgIds;

		size_t numHops = hops.size();

		hops.emplace_front(m_ctx.getMyRouterIdentity().getHash());

		uint32_t x;

		rng.randomize((unsigned char *)&x, sizeof(uint32_t));
		tunnelIds.push_back(x);

		for(int i = 0; i < numHops; i++) {
			rng.randomize((unsigned char *)&x, sizeof(uint32_t));
			tunnelIds.push_back(x);

			tunnelLayerKeys.emplace_back();
			rng.randomize(tunnelLayerKeys.back().data(), tunnelLayerKeys.back().size());

			tunnelIVKeys.emplace_back();
			rng.randomize(tunnelIVKeys.back().data(), tunnelIVKeys.back().size());

			replyKeys.emplace_back();
			rng.randomize(replyKeys.back().data(), replyKeys.back().size());

			replyIVs.emplace_back();
			rng.randomize(replyIVs.back().data(), replyIVs.back().size());

			rng.randomize((unsigned char *)&x, sizeof(uint32_t));
			nextMsgIds.push_back(x);
		}

		auto hop = hops.cbegin();
		auto tunnelId = tunnelIds.cbegin();
		auto tunnelLayerKey = tunnelLayerKeys.cbegin();
		auto tunnelIVKey = tunnelIVKeys.cbegin();
		auto replyKey = replyKeys.cbegin();
		auto replyIV = replyIVs.cbegin();
		auto nextMsgId = nextMsgIds.cbegin();

		m_records.emplace_front(
				*tunnelId++,
				*hop++,
				0,
				RouterHash(),
				SessionKey(),
				SessionKey(),
				SessionKey(),
				SessionKey(),
				BuildRequestRecord::HopType::PARTICIPANT,
				0,
				0);
		m_records.front().encrypt(m_ctx.getMyRouterIdentity().getEncryptionKey());

		uint32_t hoursSinceEpoch = std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();

		for(int i = 0; i < numHops; i++) {
			BuildRequestRecord::HopType htype;
			if(!i) {
				htype = BuildRequestRecord::HopType::INBOUND_GW;
				m_inboundGateway = *hop;
			} else
				htype = BuildRequestRecord::HopType::PARTICIPANT;

			m_records.emplace_front(	// We're adding to the front so future
					*tunnelId,						// iterations begin with the IBGW.
					*hop,
					*prev(tunnelId++),
					*prev(hop),
					*tunnelLayerKey++,
					*tunnelIVKey++,
					*replyKey++,
					*replyIV++,
					htype,
					hoursSinceEpoch,
					*nextMsgId++);

			const RouterInfo&& ri = m_ctx.getDatabase().getRouterInfo(*hop++); // TODO Will core dump if hop isn't found.
			m_records.front().encrypt(ri.getIdentity().getEncryptionKey());
		}

		// This iteratively decrypts the records which
		// were asymmetrically encrypted above.
		for(auto f = m_records.begin(); f != m_records.end(); ++f) {
			std::list<BuildRequestRecord>::reverse_iterator r(f);
			for(; r != m_records.rend(); ++r)
				f->decrypt(r->getReplyIV(), r->getReplyKey());
		}
	}

	void InboundTunnel::sendRequest()
	{
		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(m_records));
		m_ctx.getOutMsgDispatcher().sendMessage(m_inboundGateway, vtb);
	}
}
