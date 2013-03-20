#ifndef UDPTRANSPORT_H
#define UDPTRANSPORT_H

#include <thread>

#include <boost/asio.hpp>

#include "../datatypes/SessionKey.h"

#include "../Log.h"

#include "Transport.h"

#include "ssu/Packet.h"
#include "ssu/PacketHandler.h"
#include "ssu/PeerStateList.h"

namespace i2pcpp {
	class UDPTransport : public Transport {
		friend class SSU::PacketHandler;

		public:
			UDPTransport(SessionKey const &sk);
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

			i2p_logger_mt& getLogger();
			const SessionKey& getInboundKey();

			boost::asio::io_service m_ios;
			boost::asio::ip::udp::socket m_socket;
			boost::asio::ip::udp::endpoint m_senderEndpoint;

			std::thread m_serviceThread;

			std::array<unsigned char, 1024> m_receiveBuf;

			SSU::PeerStateList m_peers;
			SessionKey m_inboundKey;

			SSU::PacketHandler m_packetHandler;

			i2p_logger_mt m_log;
	};
}

#endif
