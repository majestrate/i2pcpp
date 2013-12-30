/**
 * @file UDPTransport.h
 * @brief Defines the i2pcpp::UDPTransport class. 
 */
#ifndef UDPTRANSPORT_H
#define UDPTRANSPORT_H

#include <thread>

#include <boost/asio.hpp>

#include "../datatypes/RouterIdentity.h"

#include "../Log.h"

#include "Transport.h"

#include "ssu/Packet.h"
#include "ssu/PacketHandler.h"
#include "ssu/PeerStateList.h"
#include "ssu/EstablishmentManager.h"
#include "ssu/AcknowledgementManager.h"
#include "ssu/OutboundMessageFragments.h"

namespace i2pcpp {

    /**
     * Provides SSU functionality.
     */
    class UDPTransport : public Transport {
        friend class SSU::PacketHandler;
        friend class SSU::PeerStateList;
        friend class SSU::EstablishmentManager;
        friend class SSU::InboundMessageFragments;
        friend class SSU::AcknowledgementManager;
        friend class SSU::OutboundMessageFragments;

        public:
            UDPTransport(Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri);
            ~UDPTransport();

            /**
             * Starts the transport. That is, binds the socket to the
             *  i2pcpp::Endpoint and then starts receiving data.
             * @param ep the i2pcpp::Endpoint to listen on
             */
            void start(Endpoint const &ep);
            
            /**
             * Iterates over all addresses listed in the i2pcpp::RouterInfo, and
             *  attempts to establish a session with the first one that has SSU
             *  as the transport.
             */
            void connect(RouterInfo const &ri);

            void send(RouterHash const &rh, uint32_t msgId, ByteArray const &data);

            /**
             * Disconnects the peer given by i2pcpp::RouterHash \a rh.
             */
            void disconnect(RouterHash const &rh);

            uint32_t numPeers() const;

            bool isConnected(RouterHash const &rh) const;

            /**
             * Stops the transport. That is, iterates over all connected peers and sends
             *  them a session destroyed i2pcpp::Destroyed. Then stops the IO service
             *  and joins all of the service threads.
             */
            void shutdown();

        private:
            /**
             * Sends an i2pcpp::Packet.
             * @param p a pointer to the i2pcpp::Packet to be send
             * @note the endpoint is enclosed in the i2pcpp::Packet.
             */
            void sendPacket(SSU::PacketPtr const &p);

            /**
             * Called when an i2pcpp::ReceivedSignal occurs. Builds an i2pcpp::Packet
             *  from the receieved data in i2pcpp::UDPTransport::m_receiveBuf and passes
             *  it on to the i2pcpp::UDPTranport:m_packetHandler. 
             * @param e error code that may indicate the nature of failure 
             * @param n the amount of bytes received
             */
            void dataReceived(const boost::system::error_code& e, size_t n);

            /**
             * Called when an i2pcpp::ReceivedSignal occurs. 
             * @param e error code that may indicate the nature of failure 
             * @param n the amount of bytes received
             * @param ep the UDP endpoint involved
             */
            void dataSent(const boost::system::error_code& e, size_t n, boost::asio::ip::udp::endpoint ep);

            /**
             * @return a reference to the i2cpp::SSU::EstablishmentManager
             */
            SSU::EstablishmentManager& getEstablisher();

            boost::asio::io_service m_ios;
            boost::asio::ip::udp::socket m_socket;
            boost::asio::ip::udp::endpoint m_senderEndpoint;

            /// Buffer to store receieved data in 
            std::array<unsigned char, 2048> m_receiveBuf;

            std::thread m_serviceThread;

            /// Keeps a list of connected peers
            SSU::PeerStateList m_peers;

            /// Handles received i2pcpp::Packet objects
            SSU::PacketHandler m_packetHandler;

            /// Manages connection establishment
            SSU::EstablishmentManager m_establishmentManager;

            SSU::AcknowledgementManager m_ackManager;

            /// Manages sending of outbound messages
            SSU::OutboundMessageFragments m_omf;

            /// Logging object
            i2p_logger_mt m_log;
    };
}

#endif
