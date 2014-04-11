#ifndef SERVER_H
#define SERVER_H

#include "Logger.h"

#include <i2pcpp/datatypes/Endpoint.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/connection.hpp>

#include <boost/asio.hpp>

#include <set>
#include <thread>
#include <mutex>
#include <memory>

namespace wspp = websocketpp;

class Server {
    public:
        Server(i2pcpp::Endpoint const &ep);
        ~Server();

        void run();
        void stop();

    private:
        void on_open(wspp::connection_hdl handle);
        void on_close(wspp::connection_hdl handle);

        void broadcastStats(stats_t & stats);
        void timerCallback(const boost::system::error_code &e);

        i2pcpp::Endpoint m_endpoint;

        std::thread m_serviceThread;
        boost::asio::io_service m_ios;

        typedef wspp::server<wspp::config::asio> server_t;
        server_t m_server;

        std::set<wspp::connection_hdl, std::owner_less<wspp::connection_hdl>> m_controlClients;
        std::set<wspp::connection_hdl, std::owner_less<wspp::connection_hdl>> m_statsClients;

        std::mutex m_connectionsMutex;

        boost::shared_ptr<StatsBackend> m_stats;
        boost::asio::deadline_timer m_statsTimer;

        i2p_logger_mt m_log;
};

#endif
