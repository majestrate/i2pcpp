#ifndef SSUCONTEXT_H
#define SSUCONTEXT_H

#include "PeerStateList.h"
#include "PacketHandler.h"
#include "EstablishmentManager.h"
#include "AcknowledgementManager.h"
#include "OutboundMessageFragments.h"
#include "PacketBuilder.h"

#include <i2pcpp/Log.h>

#include <boost/asio.hpp>

#include <thread>

namespace i2pcpp {
    namespace SSU {
        struct Context {
            Context(std::string const &privKeyPEM, RouterIdentity const &ri);

            /**
             * Sends an i2pcpp::Packet.
             * @param p a pointer to the i2pcpp::Packet to be send
             * @note the endpoint is enclosed in the i2pcpp::Packet.
             */
            void sendPacket(PacketPtr const &p);

            /**
             * Called when an i2pcpp::ReceivedSignal occurs. Builds an i2pcpp::Packet
             * from the receieved data in i2pcpp::UDPTransport::m_receiveBuf
             * and passes it on to the i2pcpp::UDPTranport:m_packetHandler.
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
    }
}

#endif
