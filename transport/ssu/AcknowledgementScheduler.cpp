#include "AcknowledgementScheduler.h"

#include <boost/bind.hpp>

#include "../UDPTransport.h"

#include "Packet.h"
#include "PacketBuilder.h"
#include "PeerState.h"
#include "InboundMessageState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		AcknowledgementScheduler::AcknowledgementScheduler(UDPTransport &transport) :
			m_transport(transport)
		{
			AcknowledgementTimerPtr timer(new boost::asio::deadline_timer(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 1)));

			timer->async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error, timer));
		}

		void AcknowledgementScheduler::flushAckCallback(const boost::system::error_code& e, AcknowledgementTimerPtr &timer)
		{
			for(auto& peerPair: m_transport.m_peers) {
				PeerStatePtr ps = peerPair.second;

				std::lock_guard<std::mutex> lock(ps->getMutex());

				CompleteAckList completeAckList;
				PartialAckList partialAckList;
				for(auto itr = ps->begin(); itr != ps->end();) {
					if(itr->second->allFragmentsReceived()) {
						completeAckList.push_back(itr->first);
						ps->delInboundMessageState(itr++);
						continue;
					}

					partialAckList[itr->first] = itr->second->getAckStates();

					++itr;
				}

				if(completeAckList.size() || partialAckList.size()) {
					std::forward_list<OutboundMessageState::FragmentPtr> emptyFragList;
					PacketPtr p = PacketBuilder::buildData(ps, false, emptyFragList, completeAckList, partialAckList);
					p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
					m_transport.sendPacket(p);
				}
			}

			timer->expires_at(timer->expires_at() + boost::posix_time::time_duration(0, 0, 1));
			timer->async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error, timer));
		}
	}
}
