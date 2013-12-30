#include "Server.h"

#include "Log.h"

namespace i2pcpp {
    namespace Control {
        Server::Server(Endpoint const &ep) :
            m_endpoint(ep),
            m_log(boost::log::keywords::channel = "S")
        {
            m_server.init_asio(&m_ios);

            m_server.set_open_handler(std::bind(&Server::on_open, this, std::placeholders::_1));
            m_server.set_close_handler(std::bind(&Server::on_close, this, std::placeholders::_1));

            m_backend = boost::make_shared<LoggingBackend>(*this);
            Log::addControlServerSink(m_backend);
        }

        Server::~Server()
        {
            if(m_serviceThread.joinable()) m_serviceThread.join();
        }

        void Server::run()
        {
            m_serviceThread = std::thread([&](){
                while(1) {
                    try {
                        m_server.listen(m_endpoint.getTCPEndpoint());
                        m_server.start_accept();
                        // Will an exception break things?
                        m_server.run();

                        break;
                    } catch(std::exception &e) {
                        // TODO Backtrace
                        I2P_LOG(m_log, error) << "exception in service thread: " << e.what();
                    }
                }
            });
        }

        void Server::stop()
        {
            m_server.stop();
        }

        void Server::broadcast(std::string const &data)
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            for(auto& c: m_connections)
                m_server.send(c, data, websocketpp::frame::opcode::text);
        }

        void Server::on_open(websocketpp::connection_hdl handle)
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            m_connections.insert(handle);
        }

        void Server::on_close(websocketpp::connection_hdl handle)
        {
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            m_connections.erase(handle);
        }
    }
}
