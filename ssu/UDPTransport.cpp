#include "UDPTransport.h"

#include <iostream>

#include <boost/bind.hpp>

#include "../Database.h"

namespace i2pcpp {
	namespace SSU {
		UDPTransport::UDPTransport(RouterContext &ctx) :
			Transport(ctx),
			m_socket(m_ios),
			m_packetHandler(*this),
	 		m_establisher(*this),
	 		m_receiver(*this),
			m_sender(*this),
	 		m_ackScheduler(*this) {}

		void UDPTransport::start(Endpoint const &ep)
		{
			m_endpoint = ep.getUDPEndpoint();
			m_socket.open(boost::asio::ip::udp::v4()); // TODO Support v6 too
			m_socket.bind(m_endpoint);

			m_socket.async_receive_from(
					boost::asio::buffer(m_receiveBuf.data(), BUFSIZE),
					m_senderEndpoint,
					boost::bind(
						&UDPTransport::dataReceived,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
						)
					);

			m_serviceThread = std::thread([&](){m_ios.run();});
		}

		UDPTransport::~UDPTransport()
		{
			for(auto& pair: m_peers) {
				PeerStatePtr ps = pair.second;
				PacketPtr sdp = PacketBuilder::buildSessionDestroyed(ps);
				sdp->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
				sendPacket(sdp);
			}

			m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
			m_ios.stop();
			m_serviceThread.join();
		}

		void UDPTransport::connect(RouterHash const &rh)
		{
			const RouterInfo&& ri = m_ctx.getDatabase().getRouterInfo(rh);
			m_establisher.establish(ri);
		}

		void UDPTransport::send(RouterHash const &rh, I2NP::MessagePtr const &msg)
		{
			PeerStatePtr ps = m_peers.getRemotePeer(rh);

			if(ps) {
				OutboundMessageStatePtr oms(new OutboundMessageState(msg));
				m_sender.addMessage(ps, oms);
			} else {
				// TODO Exception
			}
		}

		void UDPTransport::disconnect(RouterHash const &rh)
		{
		}

		void UDPTransport::sendPacket(PacketPtr const &p)
		{
			ByteArray pdata = p->getData();
			Endpoint ep = p->getEndpoint();

			m_socket.async_send_to(
					boost::asio::buffer(pdata.data(), pdata.size()),
					ep.getUDPEndpoint(),
					boost::bind(
						&UDPTransport::dataSent,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred,
						ep.getUDPEndpoint())
					);
		}

		void UDPTransport::dataReceived(const boost::system::error_code& e, size_t n)
		{
			if(!e && n > 0) {
				std::cerr << "UDPTransport: received " << n << " bytes from " << m_senderEndpoint << "\n";
				PacketPtr p(new Packet(Endpoint(m_senderEndpoint), m_receiveBuf.data(), n));
				m_ios.post(boost::bind(&PacketHandler::packetReceived, &m_packetHandler, p));

				m_socket.async_receive_from(
						boost::asio::buffer(m_receiveBuf.data(), BUFSIZE),
						m_senderEndpoint,
						boost::bind(
							&UDPTransport::dataReceived,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
							)
						);
			}
		}

		void UDPTransport::dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep)
		{
			std::cerr << "UDPTransport: sent " << n << " bytes to " << ep << "\n";
		}
	}
}
