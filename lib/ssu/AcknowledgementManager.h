/**
 * @file AcknowledgementManager.h
 * @brief Defines i2cpp::SSU::AcknowledgmentManager.
 */
#ifndef SSUACKNOWLEDGEMENTMANAGER_H
#define SSUACKNOWLEDGEMENTMANAGER_H

#include "../i2p/Log.h" // XXX BAD

#include <boost/asio.hpp>

namespace i2pcpp {
    class UDPTransport;

    namespace SSU {

        /**
         * Manages acknowledgment (ACK) of receieved data.
         */
        class AcknowledgementManager {
            public:

                /**
                 * Constructs given a reference to an i2pcpp::UDPTranport object.
                 */
                AcknowledgementManager(UDPTransport &transport);

                AcknowledgementManager(const AcknowledgementManager &) = delete;
                AcknowledgementManager& operator=(AcknowledgementManager &) = delete;

            private:
                /**
                 * For each peer, sends a data packet to acknowedge the fragments
                 *  (both partial and complete) that have been received from it.
                 * This is invoked exactly once every second.
                 */
                void flushAckCallback(const boost::system::error_code& e);

                /// Reference to the i2pcpp::UDPTransport object.
                UDPTransport& m_transport;

                /// Timer to invoke the ACK callback.
                boost::asio::deadline_timer m_timer;

                i2p_logger_mt m_log;
        };
    }
}

#endif
