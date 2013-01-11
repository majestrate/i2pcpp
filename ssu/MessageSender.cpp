#include "MessageSender.h"

#include <forward_list>

#include <boost/bind.hpp>

#include "PacketBuilder.h"
#include "UDPTransport.h"
#include "PeerState.h"

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		MessageSender::MessageSender(UDPTransport &transport) :
	 		m_transport(transport) {}

		void MessageSender::messageSent(PeerStatePtr &ps, OutboundMessageStatePtr &oms)
		{
			const OutboundMessageState::FragmentPtr fragment = oms->getNextFragment();
			std::forward_list<OutboundMessageState::FragmentPtr> fragList;
			fragList.push_front(fragment);

			AckList ackList;

			for(auto itr = ps->begin(); itr != ps->end();) {
				ackList.push_front(std::make_pair(itr->first, itr->second->getAckStates()));
				if(itr->second->allFragmentsReceived()) {
					ps->delInboundMessageState(itr++);
					continue;
				}

				++itr;
			}

			PacketPtr p = PacketBuilder::buildData(ps, false, fragList, ackList);
			p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
			m_transport.sendPacket(p);

			oms->markFragmentSent(fragment->fragNum);

			if(!oms->allFragmentsSent())
				addMessage(ps, oms);

			std::cerr << "MessageSender: sent packet of size " << p->getData().size() << " to " << p->getEndpoint().toString() << "\n";
		}

		void MessageSender::addMessage(PeerStatePtr const &ps, OutboundMessageStatePtr const &oms)
		{
			m_transport.m_ios.post(boost::bind(&MessageSender::messageSent, this, ps, oms));
		}
	}
}
