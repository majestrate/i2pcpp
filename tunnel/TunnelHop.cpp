#include "TunnelHop.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
	TunnelHop::TunnelHop() {}

	TunnelHop::TunnelHop(RouterIdentity const &local, RouterHash const &nextHash) :
		m_localHash(local.getHash()),
		m_nextHash(nextHash),
		m_encryptionKey(local.getEncryptionKey()),
		m_requestTime(std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count())
	{
		Botan::AutoSeeded_RNG rng;

		rng.randomize((unsigned char *)&m_nextTunnelId, sizeof(m_nextTunnelId));

		randomize();
	}

	TunnelHop::TunnelHop(RouterIdentity const &local, RouterHash const &nextHash, uint32_t const nextTunnelId) :
		m_localHash(local.getHash()),
		m_nextHash(nextHash),
		m_nextTunnelId(nextTunnelId),
		m_encryptionKey(local.getEncryptionKey()),
		m_requestTime(std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count())
	{
		randomize();
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

	void TunnelHop::setRequestTime(uint32_t reqTime)
	{
		m_requestTime = reqTime;
	}

	void TunnelHop::setNextMsgId(uint32_t nextMsgId)
	{
		m_nextMsgId = nextMsgId;
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

	uint32_t TunnelHop::getRequestTime() const
	{
		return m_requestTime;
	}

	uint32_t TunnelHop::getNextMsgId() const
	{
		return m_nextMsgId;
	}

	ByteArray TunnelHop::getEncryptionKey() const
	{
		return m_encryptionKey;
	}

	void TunnelHop::randomize()
	{
		Botan::AutoSeeded_RNG rng;

		rng.randomize((unsigned char *)&m_tunnelId, sizeof(m_tunnelId));
		rng.randomize(m_tunnelLayerKey.data(), m_tunnelLayerKey.size());
		rng.randomize(m_tunnelIVKey.data(), m_tunnelIVKey.size());
		rng.randomize(m_replyKey.data(), m_replyKey.size());
		rng.randomize(m_replyIV.data(), m_replyIV.size());
		rng.randomize((unsigned char *)&m_nextMsgId, sizeof(m_nextMsgId));
	}
}
