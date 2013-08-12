#include "AcknowledgementScheduler.h"

#include <boost/bind.hpp>

#include "../UDPTransport.h"

#include "Packet.h"
#include "PacketBuilder.h"
#include "PeerState.h"
#include "InboundMessageState.h"
#include "OutboundMessageState.h"

// TODO: figure out why this doesn't work with anything but 1
#define PUMP_INTERVAL 1

namespace i2pcpp {
	namespace SSU {
		AcknowledgementScheduler::AcknowledgementScheduler(UDPTransport &transport) :
			m_transport(transport),
			m_log(boost::log::keywords::channel = "AckS"),
			m_timer(m_transport.m_ios, boost::posix_time::time_duration(0, 0, PUMP_INTERVAL))
		{
			m_timer.async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error));
		}

		void AcknowledgementScheduler::flushAckCallback(const boost::system::error_code& e)
		{

			for(auto& peerPair: m_transport.m_peers) {
				PeerStatePtr ps = peerPair.second;
				
				std::lock_guard<std::mutex> lock(ps->getMutex());

				auto rh = ps->getIdentity().getHash();

				I2P_LOG_SCOPED_RH(m_log,rh);

	
				if (m_transport.isConnected(rh)) {
					if ( ps->keepAliveTimedOut() ) { 
						m_transport.invokeTimeoutSignal(rh);
					} else if ( ps->needsKeepAlive()) {
						ps->sentKeepAlive();
						PacketPtr p = PacketBuilder::buildKeepAlive(ps->getEndpoint());
						I2P_LOG(m_log,debug) << "send keepalive";
						p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
						m_transport.sendPacket(p);
					}
				} else {
					I2P_LOG(m_log,warning) << "trying to send when not connected";
					return;
				}
				

				CompleteAckList completeAckList;
				PartialAckList partialAckList;
				// iterate over all message states (?)
				for(auto itr = ps->begin(); itr != ps->end();) {
					if(itr->second->allFragmentsReceived()) {
						completeAckList.push_back(itr->first);
						ps->delInboundMessageState(itr++);
					} else {
						partialAckList[itr->first] = itr->second->getFragmentsReceived();
						++itr;
					}
				}
				
				if(completeAckList.size() > 0  || partialAckList.size() > 0) {
					std::vector<PacketBuilder::FragmentPtr> emptyFragList;
					PacketPtr p = PacketBuilder::buildData(ps->getEndpoint(), false, completeAckList, partialAckList, emptyFragList);
					p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
					I2P_LOG(m_log,debug) << "sending data packet, CA="<< completeAckList.size() <<" PA=" << partialAckList.size();
					m_transport.sendPacket(p);
				}
				

			}

			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, PUMP_INTERVAL));
			m_timer.async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error));
		}
	}
}
