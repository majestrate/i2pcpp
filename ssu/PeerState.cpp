#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
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

		/*OutboundMessageStatePtr PeerState::popOutboundMessageState()
		{
			return m_outboundMessageStates.pop();
		}

		void PeerState::addOutboundMessageState(OutboundMessageStatePtr const &oms)
		{
			m_outboundMessageStates.enqueue(oms);
		}

		bool PeerState::haveOutboundWaiting() const
		{
			return m_outboundMessageStates.isEmpty();
		}*/
	}
}
