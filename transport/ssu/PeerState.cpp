#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		PeerState::PeerState(Endpoint const &ep, RouterIdentity const &ri) :
			m_endpoint(ep),
			m_identity(ri) {}

		InboundMessageStatePtr PeerState::getInboundMessageState(const uint32_t msgId) const
		{
			InboundMessageStatePtr ims;

			auto itr = m_inboundMessageStates.find(msgId);
			if(itr != m_inboundMessageStates.end())
				ims = itr->second;

			return ims;
		}

		void PeerState::addInboundMessageState(InboundMessageStatePtr const &ims)
		{
			m_inboundMessageStates[ims->getMsgId()] = ims;
		}

		void PeerState::delInboundMessageState(const uint32_t msgId)
		{
			m_inboundMessageStates.erase(msgId);
		}

		void PeerState::delInboundMessageState(std::map<uint32_t, InboundMessageStatePtr>::const_iterator itr)
		{
			m_inboundMessageStates.erase(itr);
		}

		SessionKey PeerState::getCurrentSessionKey() const
		{
			return m_sessionKey;
		}

		SessionKey PeerState::getCurrentMacKey() const
		{
			return m_macKey;
		}

		SessionKey PeerState::getNextSessionKey() const
		{
			return m_nextSessionKey;
		}

		SessionKey PeerState::getNextMacKey() const
		{
			return m_nextMacKey;
		}

		void PeerState::setCurrentSessionKey(SessionKey const &sk)
		{
			m_sessionKey = sk;
		}

		void PeerState::setCurrentMacKey(SessionKey const &mk)
		{
			m_macKey = mk;
		}

		void PeerState::setNextSessionKey(SessionKey const &sk)
		{
			m_nextSessionKey = sk;
		}

		void PeerState::setNextMacKey(SessionKey const &mk)
		{
			m_nextMacKey = mk;
		}

		const RouterIdentity& PeerState::getIdentity() const
		{
			return m_identity;
		}

		const Endpoint& PeerState::getEndpoint() const
		{
			return m_endpoint;
		}
	}
}
