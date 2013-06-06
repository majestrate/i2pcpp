#include "UDPTransport.h"

#include <boost/exception/all.hpp>

#include "ssu/PeerState.h"
#include "ssu/OutboundMessageState.h"
#include "ssu/PacketBuilder.h"

namespace i2pcpp {
	UDPTransport::UDPTransport(Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri) :
		m_socket(m_ios),
		m_packetHandler(*this, ri.getHash()),
		m_establishmentManager(*this, privKey, ri),
		m_ackScheduler(*this),
		m_omf(*this),
		m_log(boost::log::keywords::channel = "SSU") {}

	UDPTransport::~UDPTransport()
	{
		shutdown();
	}

	void UDPTransport::start(Endpoint const &ep)
	{
		try {
			if(ep.getUDPEndpoint().address().is_v4())
				m_socket.open(boost::asio::ip::udp::v4());
			else if(ep.getUDPEndpoint().address().is_v6())
				m_socket.open(boost::asio::ip::udp::v6());

			m_socket.bind(ep.getUDPEndpoint());

			I2P_LOG(m_log, info) << "listening on " << ep;

			m_socket.async_receive_from(
					boost::asio::buffer(m_receiveBuf.data(), m_receiveBuf.size()),
					m_senderEndpoint,
					boost::bind(
						&UDPTransport::dataReceived,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
						)
					);

			m_serviceThread = std::thread([&](){
				while(1) {
					try {
						m_ios.run();
						break;
					} catch(std::exception &e) {
						// TODO Handle exception
						I2P_LOG(m_log, error) << "exception in service thread: " << e.what();
					}
				}
			});
		} catch(boost::system::system_error &e) {
			shutdown();
			throw;
		}
	}

	void UDPTransport::connect(RouterInfo const &ri)
	{
		try {
			for(auto a: ri) {
				if(a.getTransport() == "SSU") {
					const Mapping& m = a.getOptions();
					Endpoint ep(m.getValue("host"), stoi(m.getValue("port")));
					RouterIdentity id = ri.getIdentity();

					if(m_establishmentManager.stateExists(ep) || m_peers.remotePeerExists(ep))
						return;

					m_establishmentManager.createState(ep, id);

					I2P_LOG_SCOPED_EP(m_log, ep);
					I2P_LOG_SCOPED_RH(m_log, id.getHash());
					I2P_LOG(m_log, debug) << "attempting to establish session";

					break;
				}
			}
		} catch(std::exception &e) {
			I2P_LOG_SCOPED_TAG(m_log, "channel");
			I2P_LOG(m_log, error) << "exception thrown: " << e.what();
		}
	}

	void UDPTransport::send(RouterHash const &rh, ByteArray const &data)
	{
		using namespace SSU;

		PeerStatePtr ps = m_peers.getRemotePeer(rh);

		if(ps) {
			m_omf.sendData(ps, data);
		} else {
			// TODO Exception
		}
	}

	void UDPTransport::disconnect(RouterHash const &rh)
	{
	}

	uint32_t UDPTransport::numPeers() const
	{
		return m_peers.numPeers();
	}

	void UDPTransport::shutdown()
	{
		for(auto& pair: m_peers) {
			SSU::PacketPtr sdp = SSU::PacketBuilder::buildSessionDestroyed(pair.second->getEndpoint());
			sdp->encrypt(pair.second->getCurrentSessionKey(), pair.second->getCurrentMacKey());
			sendPacket(sdp);
		}

		m_ios.stop();
		if(m_serviceThread.joinable()) m_serviceThread.join();
	}

	void UDPTransport::sendPacket(SSU::PacketPtr const &p)
	{
		ByteArray& pdata = p->getData();
		Endpoint ep = p->getEndpoint();

		m_socket.async_send_to(
				boost::asio::buffer(pdata.data(), pdata.size()),
				ep.getUDPEndpoint(),
				boost::bind(
					&UDPTransport::dataSent,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					ep.getUDPEndpoint()
					)
				);
	}

	void UDPTransport::dataReceived(const boost::system::error_code& e, size_t n)
	{
		if(!e && n > 0) {
			Endpoint ep(m_senderEndpoint);

			I2P_LOG_SCOPED_EP(m_log, ep);
			I2P_LOG(m_log, debug) << "received " << n << " bytes from " << ep;

			if(n < SSU::Packet::MIN_PACKET_LEN) {
				I2P_LOG(m_log, debug) << "dropping short packet";
				return;
			}
			
			auto p = std::make_shared<SSU::Packet>(ep, m_receiveBuf.data(), n);
			m_ios.post(boost::bind(&SSU::PacketHandler::packetReceived, &m_packetHandler, p));

			m_socket.async_receive_from(
					boost::asio::buffer(m_receiveBuf.data(), m_receiveBuf.size()),
					m_senderEndpoint,
					boost::bind(
						&UDPTransport::dataReceived,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
						)
					);
		} else {
			I2P_LOG(m_log, debug) << "error: " << e.message();
		}
	}

	void UDPTransport::dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep)
	{
		I2P_LOG(m_log, debug) << "sent " << n << " bytes to " << ep;
	}

	SSU::EstablishmentManager& UDPTransport::getEstablisher()
	{
		return m_establishmentManager;
	}
}
