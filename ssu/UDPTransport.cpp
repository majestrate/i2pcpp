#include "UDPTransport.h"

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
			m_establisher(*this) {}

		void UDPTransport::begin(Endpoint const &ep)
		{
			m_endpoint = ep.getUDPEndpoint();
			m_socket.open(boost::asio::ip::udp::v4());
			m_socket.bind(m_endpoint);

			m_receiver.start();
			m_sender.start();
			m_handler.start();
			m_establisher.start();
		}

		void UDPTransport::connect(RouterHash const &rh)
		{
			RouterInfo ri = m_ctx.getRouterInfo(rh);
			m_establisher.establish(ri);
		}

		void UDPTransport::send(RouterHash const &rh, ByteArray const &data)
		{
		}

		void UDPTransport::disconnect(RouterHash const &rh)
		{
		}

		void UDPTransport::addRemotePeer(PeerStatePtr const &ps)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);
			m_remotePeers[ps->getEndpoint()] = ps;
		}

		PeerStatePtr UDPTransport::getRemotePeer(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				ps = itr->second;

			return ps;
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
		}
	}
}
