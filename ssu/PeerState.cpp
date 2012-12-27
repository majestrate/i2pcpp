#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageStatePtr PeerState::getInboundMessageState(const unsigned long msgId)
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
	}
}
