#include "UDPTransport.h"

#include "../Database.h"

#include "UDPReceiver.h"
#include "UDPSender.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"

#include <iostream>

#include "../util/Base64.h"

namespace i2pcpp {
	namespace SSU {
		UDPTransport::UDPTransport(RouterContext &ctx) :
			Transport(ctx),
			m_socket(m_ios),
			m_receiver(*this),
			m_sender(*this),
			m_handler(*this),
			m_establisher(*this),
	 		m_messageSender(*this),
	 		m_ackScheduler(*this)	{}

		void UDPTransport::start(Endpoint const &ep)
		{
			m_endpoint = ep.getUDPEndpoint();
			m_socket.open(boost::asio::ip::udp::v4());
			m_socket.bind(m_endpoint);

			m_receiver.start();
			m_sender.start();
			m_handler.start();
			m_establisher.start();
			m_messageSender.start();
			m_ackScheduler.start();
		}

		void UDPTransport::connect(RouterHash const &rh)
		{
			const RouterInfo&& ri = m_ctx.getDatabase().getRouterInfo(rh);
			m_establisher.establish(ri);
		}

		void UDPTransport::send(RouterHash const &rh, I2NP::MessagePtr const &msg)
		{
			PeerStatePtr ps = getRemotePeer(rh);

			if(ps) {
				OutboundMessageStatePtr oms(new OutboundMessageState(msg));
				m_messageSender.addWork(ps, oms);
			} else {
				// TODO Exception
			}
		}

		void UDPTransport::disconnect(RouterHash const &rh)
		{
		}

		void UDPTransport::addRemotePeer(PeerStatePtr const &ps)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);
			m_remotePeers[ps->getEndpoint()] = ps;
			m_remotePeersByHash[ps->getIdentity().getHash()] = ps;
		}

		PeerStatePtr UDPTransport::getRemotePeer(Endpoint const &ep) const
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				ps = itr->second;

			return ps;
		}

		PeerStatePtr UDPTransport::getRemotePeer(RouterHash const &rh) const
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeersByHash.find(rh);
			if(itr != m_remotePeersByHash.end())
				ps = itr->second;

			return ps;
		}

		void UDPTransport::delRemotePeer(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				m_remotePeersByHash.erase(itr->second->getIdentity().getHash());

			m_remotePeers.erase(ep);
		}

		void UDPTransport::delRemotePeer(RouterHash const &rh)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			auto itr = m_remotePeersByHash.find(rh);
			if(itr != m_remotePeersByHash.end())
				m_remotePeers.erase(itr->second->getEndpoint());

			m_remotePeersByHash.erase(rh);
		}

		std::unordered_map<RouterHash, PeerStatePtr>::const_iterator UDPTransport::begin() const
		{
			return m_remotePeersByHash.cbegin();
		}

		std::unordered_map<RouterHash, PeerStatePtr>::const_iterator UDPTransport::end() const
		{
			return m_remotePeersByHash.cend();
		}

		void UDPTransport::shutdown()
		{
			m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);

			m_inboundQueue.finish();
			m_outboundQueue.finish();

			m_receiver.stop();
			m_sender.stop();
			m_handler.stop();
			m_establisher.stop();
			m_messageSender.stop();
			m_ackScheduler.stop();
		}
	}
}
