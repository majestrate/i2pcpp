/**
 * @file PeerManager.h
 * @brief Defines the i2pcpp::PeerManager type.
 */
#ifndef PEERMANAGER_H
#define PEERMANAGER_H

#include <i2pcpp/Log.h>

#include <i2pcpp/datatypes/RouterHash.h>

#include <boost/asio.hpp>

#include <thread>

namespace i2pcpp {
    class RouterContext;

    /**
     * Manages peers. Tries to connect to as many peers as possible.
     */
    class PeerManager {
        public:
            /**
             * Constructs from a i2pcpp::RouterContext.
             * Sets the deadline duration to 5 seconds.
             */
            PeerManager(boost::asio::io_service &ios, RouterContext &ctx);
            PeerManager(const PeerManager &) = delete;
            PeerManager& operator=(PeerManager &) = delete;

            /**
             * Starts the i2pcpp::PeerManager.
             * That is, starts the deadline timer.
             */
            void begin();

            /**
             * Called when a new peer has connected.
             * @param rh the i2pcpp::RouterHash of the peer
             */
            void connected(const RouterHash rh);

            /**
             * Called when something goes wrong with a peer a new peer has connected.
             * @param rh the i2pcpp::RouterHash of the peer
             */
            void failure(const RouterHash rh);

            /**
             * Called when a peer has disconnected.
             * @param rh the i2pcpp::RouterHash of the peer
             */
            void disconnected(const RouterHash rh);

            void gracefulShutdown();

        private:
            /**
             * Called when the deadline timer expires.
             * Logs the number of peers.
             * Tries to connect to \a n random known peers, where \a n is the
             *  difference between the maximum amount of peers and the current
             *  amount.
             * The deadline duration is set to 10 seconds.
             */
            void callback(const boost::system::error_code &e);

            boost::asio::io_service& m_ios;
            RouterContext& m_ctx;

            boost::asio::deadline_timer m_timer;

            i2p_logger_mt m_log; ///< Logging object
            
            bool m_graceful;
    };
}

#endif
