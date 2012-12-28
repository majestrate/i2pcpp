#ifndef SSUUDPTRANSPORT_H
#define SSUUDPTRANSPORT_H

#include <unordered_map>
#include <mutex>

#include <boost/asio.hpp>

#include "../datatypes/Endpoint.h"
#include "../InboundMessageDispatcher.h"

#include "UDPReceiver.h"
#include "UDPSender.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "PeerState.h"
#include "Packet.h"

namespace i2pcpp {
	class RouterContext;

	namespace SSU {
		typedef LockingQueue<PacketPtr> PacketQueue;

		class UDPTransport {
			public:
				UDPTransport(RouterContext &ctx, InboundMessageDispatcher &imd);

				PacketQueue& getInboundQueue() { return m_inboundQueue; }
				PacketQueue& getOutboundQueue() { return m_outboundQueue; }
				EstablishmentManager& getEstablisher() { return m_establisher; }
				InboundMessageDispatcher& getInMsgDispatcher() { return m_inMsgDispatcher; }
				boost::asio::ip::udp::socket& getSocket() { return m_socket; }

				void addRemotePeer(PeerStatePtr const &ps);
				PeerStatePtr getRemotePeer(Endpoint const &ep);
				void begin(Endpoint const &ep);
				void shutdown();
				void send(PacketPtr const &p);
				RouterContext& getContext() const { return m_ctx; }

			private:
				void startReceiver();
				void startSender();
				void startHandler();
				void startEstablisher();

				boost::asio::io_service m_ios;
				boost::asio::ip::udp::endpoint m_endpoint;
				boost::asio::ip::udp::socket m_socket;

				RouterContext &m_ctx;
				InboundMessageDispatcher& m_inMsgDispatcher;

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
