#include "MessageSender.h"

#include <forward_list>

#include "PacketBuilder.h"
#include "UDPTransport.h"
#include "PeerState.h"

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		void MessageSender::loop()
		{
			PacketBuilder pb;

			while(m_keepRunning)
			{
				m_queue.wait();

				PeerStatePtr ps = m_queue.pop();

				if(!ps)
					continue;

				OutboundMessageStatePtr oms = ps->popOutboundMessageState();

				if(!oms)
					continue; // This should never happen

				const OutboundMessageState::FragmentPtr fragment = oms->getNextFragment();
				std::forward_list<OutboundMessageState::FragmentPtr> fragList;
				fragList.push_front(fragment);

				PacketPtr p = pb.buildData(ps, false, fragList);
				p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
				m_transport.m_outboundQueue.enqueue(p);

				oms->markFragmentSent(fragment->fragNum);

				if(!oms->allFragmentsSent()) {
					ps->addOutboundMessageState(oms);
					m_queue.enqueue(ps);
				}

				std::cerr << "MessageSender: sent packet of size " << p->getData().size() << " to " << p->getEndpoint().toString() << "\n";
			}
		}
	}
}
