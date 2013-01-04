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

				OutboundMessageStatePtr oms = m_queue.pop();

				if(!oms)
					continue;

				std::cerr << "MessageSender: got OMS\n";
				oms->fragment();

				PeerStatePtr ps = oms->getPeerState();
				PacketPtr p = pb.buildData(ps, false, oms->getFragments());

				p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());

				m_transport.m_outboundQueue.enqueue(p);

				std::cerr << "MessageSender: sent packet of size " << p->getData().size() << " to " << p->getEndpoint().toString() << "\n";
			}
		}

		void MessageSender::addMessage(OutboundMessageStatePtr const &oms)
		{
			m_queue.enqueue(oms);
		}
	}
}
