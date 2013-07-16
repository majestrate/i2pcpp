#ifndef TUNNELHOP_H
#define TUNNELHOP_H

#include "../datatypes/RouterIdentity.h"
#include "../datatypes/SessionKey.h"

namespace i2pcpp {
	class TunnelHop {
		public:
			enum Type {
				PARTICIPANT,
				GATEWAY,
				ENDPOINT
			};

			TunnelHop();
			TunnelHop(RouterIdentity const &local, RouterHash const &nextHash);

			void setTunnelId(uint32_t tunnelId);
			void setNextTunnelId(uint32_t nextTunnelId);
			void setLocalHash(RouterHash const &localHash);
			void setNextHash(RouterHash const &nextHash);
			void setTunnelLayerKey(SessionKey const &tunnelLayerKey);
			void setTunnelIVKey(SessionKey const &tunnelIVKey);
			void setReplyKey(SessionKey const &replyKey);
			void setReplyIV(SessionKey const &replyIV);
			void setType(Type type);

			uint32_t getTunnelId() const;
			uint32_t getNextTunnelId() const;
			RouterHash getLocalHash() const;
			RouterHash getNextHash() const;
			SessionKey getTunnelLayerKey() const;
			SessionKey getTunnelIVKey() const;
			SessionKey getReplyKey() const;
			SessionKey getReplyIV() const;
			Type getType() const;
			ByteArray getEncryptionKey() const;

		private:
			uint32_t m_tunnelId;
			uint32_t m_nextTunnelId;
			RouterHash m_localHash;
			RouterHash m_nextHash;
			SessionKey m_tunnelLayerKey;
			SessionKey m_tunnelIVKey;
			SessionKey m_replyKey;
			SessionKey m_replyIV;
			Type m_type = PARTICIPANT;

			ByteArray m_encryptionKey;
	};

	typedef std::shared_ptr<TunnelHop> TunnelHopPtr;
}

#endif
