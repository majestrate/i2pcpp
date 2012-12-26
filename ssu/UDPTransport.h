#ifndef SSUUDPTRANSPORT_H
#define SSUUDPTRANSPORT_H

#include <unordered_map>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include "PacketQueue.h"

#include "../datatypes/SessionKey.h"
#include "../datatypes/Endpoint.h"

#include "UDPReceiver.h"
#include "UDPSender.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "PeerState.h"

using namespace std;
using namespace boost::asio;

namespace i2pcpp {
	class I2PContext;

	namespace SSU {
		class UDPTransport {
			public:
				UDPTransport(I2PContext &ctx, Endpoint const &ep);

				PacketQueue& getInboundQueue() { return m_inboundQueue; }
				PacketQueue& getOutboundQueue() { return m_outboundQueue; }
				EstablishmentManager& getEstablisher() { return m_establisher; }
				ip::udp::socket& getSocket() { return m_socket; }
				bool keepRunning() const { return m_keepRunning; }

				void addRemotePeer(PeerStatePtr const &ps);
				PeerStatePtr getRemotePeer(Endpoint const &ep);
				void shutdown();
				void join();
				void begin();
				void send(PacketPtr const &p);
				I2PContext& getContext() const;

			private:
				void startReceiver();
				void startSender();
				void startHandler();
				void startEstablisher();

				I2PContext &m_ctx;

				io_service m_ios;
				ip::udp::endpoint m_endpoint;
				ip::udp::socket m_socket;

				UDPReceiver m_receiver;
				UDPSender m_sender;
				PacketHandler m_handler;
				EstablishmentManager m_establisher;

				thread m_receiver_thread;
				thread m_sender_thread;
				thread m_handler_thread;
				thread m_establisher_thread;

				PacketQueue m_inboundQueue;
				PacketQueue m_outboundQueue;

				unordered_map<Endpoint, PeerStatePtr> m_remotePeers;

				mutex m_remotePeersMutex;

				bool m_keepRunning;
		};
	}
}

#endif
