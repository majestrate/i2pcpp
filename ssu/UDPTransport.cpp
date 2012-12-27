#include "UDPTransport.h"

#include "UDPReceiver.h"
#include "UDPSender.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"

#include "../i2p.h"

#include <iostream>

#include "../util/Base64.h"

const string peer_hash = "zhPja0k1cboGnHbhqO50hNPTVHIRE8b4GMwi7Htey~E=";

namespace i2pcpp {
	namespace SSU {
		UDPTransport::UDPTransport(I2PContext &ctx, Endpoint const &ep) : m_socket(m_ios), m_ctx(ctx), m_keepRunning(true), m_receiver(*this), m_sender(*this), m_handler(*this), m_establisher(*this)
		{
			m_endpoint = ep.getUDPEndpoint();
			m_socket.open(ip::udp::v4());
			m_socket.bind(m_endpoint);
		}

		void UDPTransport::begin()
		{
			m_receiver_thread = thread(&UDPTransport::startReceiver, this);
			m_sender_thread = thread(&UDPTransport::startSender, this);
			m_handler_thread = thread(&UDPTransport::startHandler, this);
			m_establisher_thread = thread(&UDPTransport::startEstablisher, this);
		}

		void UDPTransport::startReceiver()
		{
			try {
				m_receiver.run();
			} catch(exception &e) {
				cerr << "UDPReceiver exception: " << e.what() << "\n";
			}
		}

		void UDPTransport::startSender()
		{
			try {
				m_sender.run();
			} catch(exception &e) {
				cerr << "UDPSender exception: " << e.what() << "\n";
			}
		}

		void UDPTransport::startHandler()
		{
			try {
				m_handler.run();
			} catch(exception &e) {
				cerr << "PacketHandler exception: " << e.what() << "\n";
			}
		}

		void UDPTransport::startEstablisher()
		{
			try {
				RouterInfo ri = m_ctx.getDatabase().getRouterInfo(peer_hash);
				m_establisher.establish(ri);

				m_establisher.run();
			} catch(exception &e) {
				cerr << "EstablishmentManager exception: " << e.what() << "\n";
			}
		}

		void UDPTransport::addRemotePeer(PeerStatePtr const &ps)
		{
			lock_guard<mutex> lock(m_remotePeersMutex);
			m_remotePeers[ps->getEndpoint()] = ps;
		}

		PeerStatePtr UDPTransport::getRemotePeer(Endpoint const &ep)
		{
			lock_guard<mutex> lock(m_remotePeersMutex);

			PeerStatePtr ps;

			auto itr = m_remotePeers.find(ep);
			if(itr != m_remotePeers.end())
				ps = itr->second;

			return ps;
		}

		void UDPTransport::shutdown()
		{
			m_keepRunning = false;
			m_socket.shutdown(ip::udp::socket::shutdown_both);
			m_inboundQueue.notify();
			m_outboundQueue.notify();
		}

		void UDPTransport::join()
		{
			m_establisher_thread.join();
			m_receiver_thread.join();
			m_handler_thread.join();
			m_sender_thread.join();
		}

		void UDPTransport::send(PacketPtr const &p)
		{
			m_outboundQueue.enqueue(p);
		}

		I2PContext& UDPTransport::getContext() const
		{
			return m_ctx;
		}
	}
}
