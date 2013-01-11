#ifndef SSUUDPTRANSPORT_H
#define SSUUDPTRANSPORT_H

#include <array>
#include <thread>

#include <boost/asio.hpp>

#include "../Transport.h"
#include "../RouterContext.h"
#include "../datatypes/Endpoint.h"

#include "Packet.h"
#include "PeerStateList.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "MessageReceiver.h"
#include "MessageSender.h"
#include "AcknowledgementScheduler.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport : public Transport {
			friend class PacketHandler;
			friend class EstablishmentManager;
			friend class InboundMessageFragments;
			friend class MessageReceiver;
			friend class MessageSender;
			friend class AcknowledgementScheduler;

			public:
				UDPTransport(RouterContext &ctx);
				~UDPTransport();

				void start(Endpoint const &ep);
				void connect(RouterHash const &rh);
				void send(RouterHash const &rh, I2NP::MessagePtr const &msg);
				void disconnect(RouterHash const &rh);

			private:
				void dataReceived(const boost::system::error_code& e, size_t n);
				void sendPacket(PacketPtr const &p);
				void dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep);

				boost::asio::io_service m_ios;
				boost::asio::ip::udp::socket m_socket;
				boost::asio::ip::udp::endpoint m_endpoint;
				boost::asio::ip::udp::endpoint m_senderEndpoint;

				std::thread m_serviceThread;

				static const unsigned int BUFSIZE = 1024;
				std::array<unsigned char, BUFSIZE> m_receiveBuf;

				PeerStateList m_peers;

				PacketHandler m_packetHandler;
				EstablishmentManager m_establisher;
				MessageReceiver m_receiver;
				MessageSender m_sender;
				AcknowledgementScheduler m_ackScheduler;
		};
	}
}

#endif
