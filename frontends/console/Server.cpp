#include "Server.h"

using namespace i2pcpp;

Server::Server(Endpoint const &ep) :
    m_endpoint(ep),
    m_statsTimer(m_ios, boost::posix_time::time_duration(0, 0, 1)),
    m_log(boost::log::keywords::channel = "S")
{
    m_server.init_asio(&m_ios);
    m_server.clear_access_channels(wspp::log::alevel::all);

    m_server.set_open_handler(std::bind(&Server::on_open, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&Server::on_close, this, std::placeholders::_1));

    m_stats = boost::make_shared<StatsBackend>();
    Logger::addBackendToAsyncSink(m_stats);

    m_statsTimer.async_wait(boost::bind(&Server::timerCallback, this, boost::asio::placeholders::error));
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

void Server::on_open(wspp::connection_hdl handle)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    auto r = con->get_resource();
    if(r == "/stats")
        m_statsClients.insert(handle);
    else if(r == "/control")
        m_controlClients.insert(handle);
}

void Server::on_close(wspp::connection_hdl handle)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    server_t::connection_ptr con = m_server.get_con_from_hdl(handle);
    auto r = con->get_resource();
    if(r == "/stats")
        m_statsClients.erase(handle);
    else if(r == "/control")
        m_controlClients.erase(handle);
}

void Server::broadcastStats(uint64_t bytesSent, uint64_t bytesReceived)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    for(auto& c: m_statsClients) {
        m_server.send(c, ("[" + std::to_string(bytesSent) + "," + std::to_string(bytesReceived) + "]"), wspp::frame::opcode::text);
    }
}

void Server::timerCallback(const boost::system::error_code &e)
{
    if(!e) {
        auto stats = m_stats->getBytesAndReset();
        broadcastStats(stats.first, stats.second);

        m_statsTimer.expires_at(m_statsTimer.expires_at() + boost::posix_time::time_duration(0, 0, 1));
        m_statsTimer.async_wait(boost::bind(&Server::timerCallback, this, boost::asio::placeholders::error));
    }
}
