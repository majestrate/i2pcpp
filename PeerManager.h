#ifndef PEERMANAGER_H
#define PEERMANAGER_H

#include <thread>

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "Log.h"

namespace i2pcpp {
    class RouterContext;

    class PeerManager {
        public:
            PeerManager(boost::asio::io_service &ios, RouterContext &ctx);
            PeerManager(const PeerManager &) = delete;
            PeerManager& operator=(PeerManager &) = delete;

            void begin();

            void connected(const RouterHash rh);
            void failure(const RouterHash rh);
            void disconnected(const RouterHash rh);

        private:
            void callback(const boost::system::error_code &e);

            boost::asio::io_service& m_ios;
            RouterContext& m_ctx;

            boost::asio::deadline_timer m_timer;

            i2p_logger_mt m_log;
    };
}

#endif
