#include "TunnelState.h"

#include <botan/auto_rng.h>

#include "../datatypes/RouterInfo.h"

#include "../RouterContext.h"

namespace i2pcpp {
	TunnelState::TunnelState(RouterContext &ctx, std::list<RouterHash> const &hops, TunnelState::Direction d) :
		m_ctx(ctx),
		m_direction(d),
		m_hops(hops)
	{
		Botan::AutoSeeded_RNG rng;

		size_t numHops = m_hops.size();

		m_hops.emplace_front(m_ctx.getIdentity().getHash());

		uint32_t x;

		rng.randomize((unsigned char *)&x, sizeof(uint32_t));
		m_tunnelIds.push_back(x);

		for(int i = 0; i < numHops; i++) {
			rng.randomize((unsigned char *)&x, sizeof(uint32_t));
			m_tunnelIds.push_back(x);

			m_tunnelLayerKeys.emplace_back();
			rng.randomize(m_tunnelLayerKeys.back().data(), m_tunnelLayerKeys.back().size());

			m_tunnelIVKeys.emplace_back();
			rng.randomize(m_tunnelIVKeys.back().data(), m_tunnelIVKeys.back().size());

			m_replyKeys.emplace_back();
			rng.randomize(m_replyKeys.back().data(), m_replyKeys.back().size());

			m_replyIVs.emplace_back();
			rng.randomize(m_replyIVs.back().data(), m_replyIVs.back().size());

			rng.randomize((unsigned char *)&x, sizeof(uint32_t));
			m_nextMsgIds.push_back(x);
		}

		switch(d) {
			case TunnelState::INBOUND:
				buildInboundRequest();
				break;

			case TunnelState::OUTBOUND:
				buildOutboundRequest();
				break;
		}
	}

	const std::list<BuildRequestRecord>& TunnelState::getRequest() const
	{
		return m_records;
	}

	uint32_t TunnelState::getTunnelId() const
	{
		return m_tunnelId;
	}

	const RouterHash& TunnelState::getTerminalHop() const
	{
		return m_terminalHop;
	}

	TunnelState::Direction TunnelState::getDirection() const
	{
		return m_direction;
	}

	void TunnelState::buildInboundRequest()
	{
		auto hop = m_hops.cbegin();
		auto tunnelId = m_tunnelIds.cbegin();
		auto tunnelLayerKey = m_tunnelLayerKeys.cbegin();
		auto tunnelIVKey = m_tunnelIVKeys.cbegin();
		auto replyKey = m_replyKeys.cbegin();
		auto replyIV = m_replyIVs.cbegin();
		auto nextMsgId = m_nextMsgIds.cbegin();

		size_t numHops = m_hops.size();

		m_records.clear();

		m_tunnelId = *tunnelId;
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
		m_records.front().encrypt(m_ctx.getIdentity().getEncryptionKey());

		uint32_t hoursSinceEpoch = std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();

		for(int i = 0; i < numHops - 1; i++) {
			BuildRequestRecord::HopType htype;
			if(i == (numHops - 2)) {
				htype = BuildRequestRecord::HopType::INBOUND_GW;
				m_terminalHop = *hop;
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

			const RouterInfo ri = m_ctx.getDatabase().getRouterInfo(*hop++); // TODO Will core dump if hop isn't found.
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

	void TunnelState::buildOutboundRequest()
	{
	}
}
