#ifndef BUILDREQUESTRECORD_H
#define BUILDREQUESTRECORD_H

#include "Datatype.h"
#include "RouterHash.h"
#include "SessionKey.h"

namespace i2pcpp {
	class BuildRequestRecord : public Datatype {
		public:
			enum HopType {
				PARTICIPANT = 0,
				INBOUND_GW = (1 << 7),
				OUTBOUND_EP = (1 << 6)
			};

			BuildRequestRecord(uint32_t tunnelId, RouterHash const &localIdentity, uint32_t nextTunnelId, RouterHash const &nextIdentity, SessionKey const &tunnelLayerKey, SessionKey const &tunnelIVKey, SessionKey const &replyKey, SessionKey const &replyIV, HopType type, uint32_t requestTime, uint32_t nextMsgId);

			ByteArray getBytes() const;

		private:
			uint32_t m_tunnelId;
			RouterHash m_localIdentity;
			uint32_t m_nextTunnelId;
			RouterHash m_nextIdentity;
			SessionKey m_tunnelLayerKey;
			SessionKey m_tunnelIVKey;
			SessionKey m_replyKey;
			SessionKey m_replyIV;
			unsigned char m_flags;
			uint32_t m_requestTime;
			uint32_t m_nextMsgId;
	};
}

#endif
