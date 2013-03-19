#ifndef UDPTRANSPORT_H
#define UDPTRANSPORT_H

#include <thread>

#include <boost/asio.hpp>

#include "Transport.h"

#include "ssu/Packet.h"
#include "ssu/PacketHandler.h"
#include "ssu/PeerStateList.h"

namespace i2pcpp {
	class UDPTransport : public Transport {
		public:
			UDPTransport();
			~UDPTransport();

			void start(Endpoint const &ep);
			void connect(RouterHash const &rh, Endpoint const &ep);
			void send(RouterHash const &rh, ByteArray const &msg);
			void disconnect(RouterHash const &rh);
			void shutdown();

		private:
			void sendPacket(SSU::PacketPtr const &p);
			void dataReceived(const boost::system::error_code& e, size_t n);
			void dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep);

			boost::asio::io_service m_ios;
			boost::asio::ip::udp::socket m_socket;
			boost::asio::ip::udp::endpoint m_senderEndpoint;

			std::thread m_serviceThread;

			std::array<unsigned char, 1024> m_receiveBuf;

			SSU::PeerStateList m_peers;

			SSU::PacketHandler m_packetHandler;
	};
}

#endif
