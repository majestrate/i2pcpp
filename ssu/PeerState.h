#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include <mutex>
#include <memory>
#include <unordered_map>

#include "InboundMessageState.h"

#include "../datatypes/RouterIdentity.h"
#include "../datatypes/Endpoint.h"
#include "../datatypes/SessionKey.h"

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

				std::mutex& getMutex() { return m_mutex; }
				Endpoint getEndpoint() const { return m_endpoint; }
				const RouterIdentity& getIdentity() { return m_identity; }

				InboundMessageStatePtr getInboundMessageState(const unsigned long msgId);
				void addInboundMessageState(InboundMessageStatePtr const &ims);

			private:
				Endpoint m_endpoint;
				RouterIdentity m_identity;
				bool m_isInbound;

				std::unordered_map<unsigned long, InboundMessageStatePtr> m_inboundMessageStates;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;

				std::mutex m_mutex;
		};

		typedef std::shared_ptr<PeerState> PeerStatePtr;
	}
}

#endif
