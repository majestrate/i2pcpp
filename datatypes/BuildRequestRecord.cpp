#include "BuildRequestRecord.h"

namespace i2pcpp {
	BuildRequestRecord::BuildRequestRecord(uint32_t tunnelId, RouterHash const &localIdentity, uint32_t nextTunnelId, RouterHash const &nextIdentity, SessionKey const &tunnelLayerKey, SessionKey const &tunnelIVKey, SessionKey const &replyKey, SessionKey const &replyIV, HopType type, uint32_t requestTime, uint32_t nextMsgId) :
		m_tunnelId(tunnelId),
		m_localIdentity(localIdentity),
		m_nextTunnelId(nextTunnelId),
		m_nextIdentity(nextIdentity),
		m_tunnelLayerKey(tunnelLayerKey),
		m_tunnelIVKey(tunnelIVKey),
		m_replyKey(replyKey),
		m_replyIV(replyIV),
		m_flags(0),
		m_requestTime(requestTime),
		m_nextMsgId(nextMsgId)
	{
		m_flags |= type;
	}

	ByteArray BuildRequestRecord::getBytes() const
	{
	}
}
