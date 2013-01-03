#ifndef SSUUDPTRANSPORT_H
#define SSUUDPTRANSPORT_H

#include <unordered_map>
#include <mutex>

#include <boost/asio.hpp>

#include "../datatypes/Endpoint.h"
#include "../Transport.h"

#include "UDPReceiver.h"
#include "UDPSender.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "PeerState.h"
#include "Packet.h"

namespace i2pcpp {
	namespace SSU {
		typedef LockingQueue<PacketPtr> PacketQueue;

		class UDPTransport : public Transport {
			friend class UDPReceiver;
			friend class UDPSender;
			friend class PacketHandler;
			friend class EstablishmentManager;

			public:
				UDPTransport(RouterContext &ctx);

				void begin(Endpoint const &ep);
				void connect(RouterHash const &rh);
				void send(RouterHash const &rh, ByteArray const &data);
				void disconnect(RouterHash const &rh);
				void shutdown();

			private:
				void startReceiver();
				void startSender();
				void startHandler();
				void startEstablisher();

				void addRemotePeer(PeerStatePtr const &ps);
				PeerStatePtr getRemotePeer(Endpoint const &ep);

				boost::asio::io_service m_ios;
				boost::asio::ip::udp::endpoint m_endpoint;
				boost::asio::ip::udp::socket m_socket;

				UDPReceiver m_receiver;
				UDPSender m_sender;
				PacketHandler m_handler;
				EstablishmentManager m_establisher;

				PacketQueue m_inboundQueue;
				PacketQueue m_outboundQueue;

				std::unordered_map<Endpoint, PeerStatePtr> m_remotePeers;

				std::mutex m_remotePeersMutex;
		};
	}
}

#endif
