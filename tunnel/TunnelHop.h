#ifndef TUNNELHOP_H
#define TUNNELHOP_H

#include <unordered_map>

#include "../datatypes/RouterIdentity.h"
#include "../datatypes/SessionKey.h"

namespace i2pcpp {
	class TunnelHop {
		public:
			enum class Type {
				PARTICIPANT,
				GATEWAY,
				ENDPOINT
			};

			TunnelHop();
			TunnelHop(RouterIdentity const &local, RouterHash const &nextHash);
			TunnelHop(RouterIdentity const &local, RouterHash const &nextHash, uint32_t const nextTunnelId);

			void setTunnelId(uint32_t tunnelId);
			void setNextTunnelId(uint32_t nextTunnelId);
			void setLocalHash(RouterHash const &localHash);
			void setNextHash(RouterHash const &nextHash);
			void setTunnelLayerKey(SessionKey const &tunnelLayerKey);
			void setTunnelIVKey(SessionKey const &tunnelIVKey);
			void setReplyKey(SessionKey const &replyKey);
			void setReplyIV(StaticByteArray<16, true> const &replyIV);
			void setType(Type type);
			void setRequestTime(uint32_t reqTime);
			void setNextMsgId(uint32_t nextMsgId);

			uint32_t getTunnelId() const;
			uint32_t getNextTunnelId() const;
			RouterHash getLocalHash() const;
			RouterHash getNextHash() const;
			SessionKey getTunnelLayerKey() const;
			SessionKey getTunnelIVKey() const;
			SessionKey getReplyKey() const;
			StaticByteArray<16, true> getReplyIV() const;
			Type getType() const;
			uint32_t getRequestTime() const;
			uint32_t getNextMsgId() const;
			ByteArray getEncryptionKey() const;

		private:
			void randomize();

			uint32_t m_tunnelId;
			uint32_t m_nextTunnelId;
			RouterHash m_localHash;
			RouterHash m_nextHash;
			SessionKey m_tunnelLayerKey;
			SessionKey m_tunnelIVKey;
			SessionKey m_replyKey;
			StaticByteArray<16, true> m_replyIV;
			Type m_type = Type::PARTICIPANT;
			uint32_t m_requestTime;
			uint32_t m_nextMsgId;

			ByteArray m_encryptionKey;
	};

	typedef std::shared_ptr<TunnelHop> TunnelHopPtr;
}

#endif
