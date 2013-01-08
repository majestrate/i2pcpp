#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include <mutex>
#include <memory>
#include <unordered_map>

#include "../datatypes/RouterIdentity.h"
#include "../datatypes/Endpoint.h"
#include "../datatypes/SessionKey.h"

#include "../util/LockingQueue.h"

#include "InboundMessageState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				PeerState(Endpoint const &ep, RouterIdentity const &ri, bool isInbound) : m_endpoint(ep), m_identity(ri), m_isInbound(isInbound) {}

				void setCurrentSessionKey(SessionKey const &sk) { m_sessionKey = sk; }
				void setCurrentMacKey(SessionKey const &mk) { m_macKey = mk; }
				void setNextSessionKey(SessionKey const &sk) { m_nextSessionKey = sk; }
				void setNextMacKey(SessionKey const &mk) { m_nextMacKey = mk; }

				SessionKey getCurrentSessionKey() const { return m_sessionKey; }
				SessionKey getCurrentMacKey() const { return m_macKey; }
				SessionKey getNextSessionKey() const { return m_nextSessionKey; }
				SessionKey getNextMacKey() const { return m_nextMacKey; }

				std::mutex& getMutex() const { return m_mutex; }
				const Endpoint& getEndpoint() const { return m_endpoint; }
				const RouterIdentity& getIdentity() const { return m_identity; }
				bool getDirection() const { return m_isInbound; }

				InboundMessageStatePtr getInboundMessageState(const uint32_t msgId) const;
				void addInboundMessageState(InboundMessageStatePtr const &ims);
				void delInboundMessageState(const uint32_t msgId);
				void delInboundMessageState(std::map<uint32_t, InboundMessageStatePtr>::const_iterator itr);
				std::map<uint32_t, InboundMessageStatePtr>::iterator begin();
				std::map<uint32_t, InboundMessageStatePtr>::iterator end();

			private:
				Endpoint m_endpoint;
				RouterIdentity m_identity;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;

				bool m_isInbound;

				std::map<uint32_t, InboundMessageStatePtr> m_inboundMessageStates;

				mutable std::mutex m_mutex;
		};

		typedef std::shared_ptr<PeerState> PeerStatePtr;
	}
}

template<>
struct std::hash<i2pcpp::SSU::PeerStatePtr> {
	public:
		size_t operator()(const i2pcpp::SSU::PeerStatePtr &ps) const
		{
			std::hash<i2pcpp::RouterHash> f;
			return f(ps->getIdentity().getHash());
		}
};

#endif
