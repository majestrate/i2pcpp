#include "UDPTransport.h"

#include <boost/exception/all.hpp>

#include "ssu/PeerState.h"
#include "ssu/OutboundMessageState.h"
#include "ssu/PacketBuilder.h"

namespace i2pcpp {
    UDPTransport::UDPTransport(Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri) :
        Transport(),
        m_socket(m_ios),
        m_peers(*this),
        m_packetHandler(*this, (SessionKey)(ByteArray)ri.getHash()),
        m_establishmentManager(*this, privKey, ri),
        m_ackManager(*this),
        m_omf(*this),
        m_log(I2P_LOG_CHANNEL("SSU")) {}

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
                        I2P_LOG(m_log, error) << "exception thrown in transport: " << e.what();
                    }
                }
            });
        } catch(boost::system::system_error &e) {
            I2P_LOG(m_log, error) << "system error in transport: " << e.what();
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

                    // Skip if the host and port are not valid. TODO Is this the best way?
                    if(!m.getValue("host").size() || !m.getValue("port").size())
                        continue;

                    Endpoint ep(m.getValue("host"), stoi(m.getValue("port")));
                    RouterIdentity id = ri.getIdentity();

                    std::lock_guard<std::mutex> lock(m_peers.getMutex());

                    if(m_establishmentManager.stateExists(ep) || m_peers.peerExists(ep))
                        return;

                    m_establishmentManager.createState(ep, id);

                    I2P_LOG_SCOPED_TAG(m_log, "Endpoint", ep);
                    I2P_LOG_SCOPED_TAG(m_log, "RouterHash", id.getHash());
                    I2P_LOG(m_log, debug) << "attempting to establish session";

                    break;
                }
            }
        } catch(std::exception &e) {
            I2P_LOG(m_log, error) << "exception thrown: " << e.what();
        }
    }

    void UDPTransport::send(RouterHash const &rh, uint32_t msgId, ByteArray const &data)
    {
        using namespace SSU;

        std::lock_guard<std::mutex> lock(m_peers.getMutex());

        if(m_peers.peerExists(rh)) {
            PeerState ps = m_peers.getPeer(rh);

            m_omf.sendData(ps, msgId, data);
        } else {
            // TODO Exception
        }
    }

    void UDPTransport::disconnect(RouterHash const &rh)
    {
        std::lock_guard<std::mutex> lock(m_peers.getMutex());

        if(m_peers.peerExists(rh)) {
            const SSU::PeerState& ps = m_peers.getPeer(rh);

            SSU::PacketPtr p = SSU::PacketBuilder::buildSessionDestroyed(ps.getEndpoint());
            p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
            sendPacket(p);

            m_peers.delPeer(rh);
        }
    }

    uint32_t UDPTransport::numPeers() const
    {
        std::lock_guard<std::mutex> lock(m_peers.getMutex());

        return m_peers.numPeers();
    }

    bool UDPTransport::isConnected(RouterHash const &rh) const
    {
        std::lock_guard<std::mutex> lock(m_peers.getMutex());

        return m_peers.peerExists(rh);
    }

    void UDPTransport::shutdown()
    {
        I2P_LOG(m_log, info) << "shutdown transport";
        std::lock_guard<std::mutex> lock(m_peers.getMutex());

        for(auto itr = m_peers.cbegin(); itr != m_peers.cend(); ++itr) {
            SSU::PacketPtr sdp = SSU::PacketBuilder::buildSessionDestroyed(itr->getEndpoint());
            sdp->encrypt(itr->state.getCurrentSessionKey(), itr->state.getCurrentMacKey());
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

            I2P_LOG_SCOPED_TAG(m_log, "Endpoint", ep);
            I2P_LOG(m_log, debug) << "received " << n << " bytes";

            if(n >= SSU::Packet::MIN_PACKET_LEN) {
                auto p = std::make_shared<SSU::Packet>(ep, m_receiveBuf.data(), n);
                m_ios.post(boost::bind(&SSU::PacketHandler::packetReceived, &m_packetHandler, p));
            } else
                I2P_LOG(m_log, debug) << "dropping short packet";

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
            I2P_LOG(m_log, debug) << "error when recieving: " << e.message();
        }
    }

    void UDPTransport::dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep)
    {
        I2P_LOG_SCOPED_TAG(m_log, "Endpoint", Endpoint(ep));
        I2P_LOG(m_log, debug) << "sent " << n << " bytes";
    }

    SSU::EstablishmentManager& UDPTransport::getEstablisher()
    {
        return m_establishmentManager;
    }
}
