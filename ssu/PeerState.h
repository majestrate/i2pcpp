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

				std::mutex& getMutex() { return m_mutex; }
				Endpoint getEndpoint() const { return m_endpoint; }
				const RouterIdentity& getIdentity() const { return m_identity; }

				InboundMessageStatePtr getInboundMessageState(const uint32_t msgId);
				void addInboundMessageState(InboundMessageStatePtr const &ims);
				void delInboundMessageState(const uint32_t msgId);

				OutboundMessageStatePtr popOutboundMessageState();
				void addOutboundMessageState(OutboundMessageStatePtr const &oms);
				bool haveOutboundWaiting() const;

				void pushAck(const uint32_t msgId) { m_ackQueue.enqueue(msgId); }
				const uint32_t popAck() { return m_ackQueue.pop(); }

			private:
				Endpoint m_endpoint;
				RouterIdentity m_identity;
				bool m_isInbound;

				std::unordered_map<uint32_t, InboundMessageStatePtr> m_inboundMessageStates;
				LockingQueue<OutboundMessageStatePtr> m_outboundMessageStates;
				LockingQueue<uint32_t> m_ackQueue;

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
