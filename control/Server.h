#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include <set>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/connection.hpp>

#include "../datatypes/Endpoint.h"

#include "../Log.h"

namespace i2pcpp {
    namespace Control {
        class Server {
            public:
                Server(Endpoint const &ep);
                ~Server();

                void run();
                void stop();

                void broadcast(std::string const &data);

            private:
                void on_open(websocketpp::connection_hdl handle);
                void on_close(websocketpp::connection_hdl handle);

                Endpoint m_endpoint;

                std::thread m_serviceThread;
                boost::asio::io_service m_ios;
                websocketpp::server<websocketpp::config::asio> m_server;
                std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> m_connections;
                std::mutex m_connectionsMutex;

                boost::shared_ptr<LoggingBackend> m_backend;

                i2p_logger_mt m_log;
        };
    }
}

#endif
