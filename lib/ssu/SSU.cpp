/**
 * @file SSU.cpp
 * @brief Implements SSU.h
 */
#include "../../include/i2pcpp/transports/SSU.h"

#include "PeerStateList.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "AcknowledgementManager.h"
#include "OutboundMessageState.h"
#include "OutboundMessageFragments.h"
#include "PacketBuilder.h"

#include <i2pcpp/Log.h>

#include <i2pcpp/util/make_unique.h>

#include <boost/asio.hpp>

#include <thread>

namespace i2pcpp {
    namespace SSU {
        struct SSU::SSUImpl {
            SSUImpl(std::string const &privKeyPEM, RouterIdentity const &ri) :
                socket(ios),
                peers(*this),
                packetHandler(*this, ri.getHash()),
                establishmentManager(*this, privKey, ri),
                ackManager(*this),
                omf(*this),
                log(boost::log::keywords::channel = "SSU") {}

            /**
             * Sends an i2pcpp::Packet.
             * @param p a pointer to the i2pcpp::Packet to be send
             * @note the endpoint is enclosed in the i2pcpp::Packet.
             */
            void sendPacket(PacketPtr const &p)
            {
                ByteArray& pdata = p->getData();
                Endpoint ep = p->getEndpoint();

                socket.async_send_to(
                        boost::asio::buffer(pdata.data(), pdata.size()),
                        ep.getUDPEndpoint(),
                        boost::bind(
                            &SSU::SSUImpl::dataSent,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred,
                            ep.getUDPEndpoint()
                            )
                        );
            }

            /**
             * Called when an i2pcpp::ReceivedSignal occurs. Builds an i2pcpp::Packet
             * from the receieved data in i2pcpp::UDPTransport::m_receiveBuf
             * and passes it on to the i2pcpp::UDPTranport:m_packetHandler.
             * @param e error code that may indicate the nature of failure
             * @param n the amount of bytes received
             */
            void dataReceived(const boost::system::error_code& e, size_t n)
            {
                if(!e && n > 0) {
                    Endpoint ep(senderEndpoint);

                    I2P_LOG_SCOPED_TAG(log, "Endpoint", ep);
                    I2P_LOG(log, debug) << "received " << n << " bytes";
                    I2P_LOG(log, debug) << boost::log::add_value("received", (uint64_t)n);

                    if(n >= Packet::MIN_PACKET_LEN) {
                        auto p = std::make_shared<Packet>(ep, receiveBuf.data(), n);
                        ios.post(boost::bind(&PacketHandler::packetReceived, &packetHandler, p));
                    } else
                        I2P_LOG(log, debug) << "dropping short packet";

                    socket.async_receive_from(
                            boost::asio::buffer(receiveBuf.data(), receiveBuf.size()),
                            senderEndpoint,
                            boost::bind(
                                &SSU::SSUImpl::dataReceived,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred
                                )
                            );
                } else {
                    I2P_LOG(log, debug) << "error: " << e.message();
                }
            }

            /**
             * Called when an i2pcpp::ReceivedSignal occurs.
             * @param e error code that may indicate the nature of failure
             * @param n the amount of bytes received
             * @param ep the UDP endpoint involved
             */
            void dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep)
            {
                I2P_LOG_SCOPED_TAG(log, "Endpoint", Endpoint(ep));
                I2P_LOG(log, debug) << "sent " << n << " bytes";
                I2P_LOG(log, debug) << boost::log::add_value("sent", (uint64_t)n);
            }

            boost::asio::io_service ios;
            boost::asio::ip::udp::socket socket;
            boost::asio::ip::udp::endpoint senderEndpoint;

            /// Buffer to store receieved data in
            std::array<unsigned char, 2048> receiveBuf;

            std::thread serviceThread;

            /// Keeps a list of connected peers
            PeerStateList peers;

            /// Handles received i2pcpp::Packet objects
            PacketHandler packetHandler;

            /// Manages connection establishment
            EstablishmentManager establishmentManager;

            AcknowledgementManager ackManager;

            /// Manages sending of outbound messages
            OutboundMessageFragments omf;

            /// Logging object
            i2p_logger_mt log;
        };

        SSU::SSU(std::string const &privKeyPEM, RouterIdentity const &ri) :
            Transport()
        {
            m_impl = std::make_unique<SSUImpl>(privKeyPEM, ri);
        }

        SSU::~SSU()
        {
            shutdown();
        }

        void SSU::start(Endpoint const &ep)
        {
            try {
                if(ep.getUDPEndpoint().address().is_v4())
                    m_impl->socket.open(boost::asio::ip::udp::v4());
                else if(ep.getUDPEndpoint().address().is_v6())
                    m_impl->socket.open(boost::asio::ip::udp::v6());

                m_impl->socket.bind(ep.getUDPEndpoint());

                I2P_LOG(m_impl->log, info) << "listening on " << ep;

                m_impl->socket.async_receive_from(
                        boost::asio::buffer(m_impl->receiveBuf.data(), m_impl->receiveBuf.size()),
                        m_impl->senderEndpoint,
                        boost::bind(
                            &SSU::SSUImpl::dataReceived,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred
                            )
                        );

                m_impl->serviceThread = std::thread([&](){
                    while(1) {
                        try {
                            m_impl->ios.run();
                            break;
                        } catch(std::exception &e) {
                            I2P_LOG(m_impl->log, error) << "exception thrown: " << e.what();
                        }
                    }
                });
            } catch(boost::system::system_error &e) {
                shutdown();
                throw;
            }
        }

        void SSU::connect(RouterInfo const &ri)
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

                        std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

                        if(m_impl->establishmentManager.stateExists(ep) || m_impl->peers.peerExists(ep))
                            return;

                        m_impl->establishmentManager.createState(ep, id);

                        I2P_LOG_SCOPED_TAG(m_impl->log, "Endpoint", ep);
                        I2P_LOG_SCOPED_TAG(m_impl->log, "RouterHash", id.getHash());
                        I2P_LOG(m_impl->log, debug) << "attempting to establish session";

                        break; // Only connect to the first address
                    }
                }
            } catch(std::exception &e) {
                I2P_LOG(m_impl->log, error) << "exception thrown: " << e.what();
            }
        }

        void SSU::send(RouterHash const &rh, uint32_t msgId, ByteArray const &data)
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            if(m_impl->peers.peerExists(rh)) {
                PeerState ps = m_impl->peers.getPeer(rh);

                m_impl->omf.sendData(ps, msgId, data);
            } else {
                // TODO Exception
            }
        }

        void SSU::disconnect(RouterHash const &rh)
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            if(m_impl->peers.peerExists(rh)) {
                const PeerState& ps = m_impl->peers.getPeer(rh);

                PacketPtr p = PacketBuilder::buildSessionDestroyed(ps.getEndpoint());
                p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
                sendPacket(p);

                m_impl->peers.delPeer(rh);
            }
        }

        uint32_t SSU::numPeers() const
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            return m_impl->peers.numPeers();
        }

        bool SSU::isConnected(RouterHash const &rh) const
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            return m_impl->peers.peerExists(rh);
        }

        void SSU::shutdown()
        {
            std::lock_guard<std::mutex> lock(m_impl->peers.getMutex());

            for(auto itr = m_impl->peers.cbegin(); itr != m_impl->peers.cend(); ++itr) {
                PacketPtr sdp = PacketBuilder::buildSessionDestroyed(itr->getEndpoint());
                sdp->encrypt(itr->state.getCurrentSessionKey(), itr->state.getCurrentMacKey());
                sendPacket(sdp);
            }

            m_impl->ios.stop();
            if(m_impl->serviceThread.joinable()) m_impl->serviceThread.join();
        }
    }
}
