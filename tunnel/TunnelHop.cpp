#include "TunnelHop.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
	TunnelHop::TunnelHop() {}

	TunnelHop::TunnelHop(RouterHash const &localHash, RouterHash const &nextHash) :
		m_localHash(localHash),
		m_nextHash(nextHash)
	{
		Botan::AutoSeeded_RNG rng;

		rng.randomize((unsigned char *)&m_tunnelId, sizeof(m_tunnelId));
		rng.randomize((unsigned char *)&m_nextTunnelId, sizeof(m_nextTunnelId));
		rng.randomize(m_tunnelLayerKey.data(), m_tunnelLayerKey.size());
		rng.randomize(m_tunnelIVKey.data(), m_tunnelIVKey.size());
		rng.randomize(m_replyKey.data(), m_replyKey.size());
		rng.randomize(m_replyIV.data(), m_replyIV.size());
	}

	void TunnelHop::setTunnelId(uint32_t tunnelId)
	{
		m_tunnelId = tunnelId;
	}

	void TunnelHop::setNextTunnelId(uint32_t nextTunnelId)
	{
		m_nextTunnelId = nextTunnelId;
	}

	void TunnelHop::setLocalHash(RouterHash const &localHash)
	{
		m_localHash = localHash;
	}

	void TunnelHop::setNextHash(RouterHash const &nextHash)
	{
		m_nextHash = nextHash;
	}

	void TunnelHop::setTunnelLayerKey(SessionKey const &tunnelLayerKey)
	{
		m_tunnelLayerKey = tunnelLayerKey;
	}

	void TunnelHop::setTunnelIVKey(SessionKey const &tunnelIVKey)
	{
		m_tunnelIVKey = tunnelIVKey;
	}

	void TunnelHop::setReplyKey(SessionKey const &replyKey)
	{
		m_replyKey = replyKey;
	}

	void TunnelHop::setReplyIV(SessionKey const &replyIV)
	{
		m_replyIV = replyIV;
	}

	void TunnelHop::setType(TunnelHop::Type type)
	{
		m_type = type;
	}

	uint32_t TunnelHop::getTunnelId() const
	{
		return m_tunnelId;
	}

	uint32_t TunnelHop::getNextTunnelId() const
	{
		return m_nextTunnelId;
	}

	RouterHash TunnelHop::getLocalHash() const
	{
		return m_localHash;
	}

	RouterHash TunnelHop::getNextHash() const
	{
		return m_nextHash;
	}

	SessionKey TunnelHop::getTunnelLayerKey() const
	{
		return m_tunnelLayerKey;
	}

	SessionKey TunnelHop::getTunnelIVKey() const
	{
		return m_tunnelIVKey;
	}

	SessionKey TunnelHop::getReplyKey() const
	{
		return m_replyKey;
	}

	SessionKey TunnelHop::getReplyIV() const
	{
		return m_replyIV;
	}

	TunnelHop::Type TunnelHop::getType() const
	{
		return m_type;
	}
}
