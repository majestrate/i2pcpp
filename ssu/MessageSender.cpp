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
			try {
				PacketBuilder pb;

				while(m_keepRunning)
				{
					auto pair = m_queue.wait_and_pop();

					PeerStatePtr ps = pair.first;
					OutboundMessageStatePtr oms = pair.second;

					std::lock_guard<std::mutex> lock(ps->getMutex());

					const OutboundMessageState::FragmentPtr fragment = oms->getNextFragment();
					std::forward_list<OutboundMessageState::FragmentPtr> fragList;
					fragList.push_front(fragment);

					PacketPtr p = pb.buildData(ps, false, fragList);
					p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
					m_transport.m_outboundQueue.enqueue(p);

					oms->markFragmentSent(fragment->fragNum);

					if(!oms->allFragmentsSent())
						m_queue.enqueue(pair);

					std::cerr << "MessageSender: sent packet of size " << p->getData().size() << " to " << p->getEndpoint().toString() << "\n";
				}
			} catch(LockingQueueFinished) {}
		}
	}
}
