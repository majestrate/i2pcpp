/**
 * @file Router.cpp
 * @brief Implements Router.h
 */
#include "../../include/i2pcpp/Router.h"

#include "../../include/i2pcpp/Transport.h"
#include "../../include/i2pcpp/Database.h"

#include "RouterContext.h"

#include <i2pcpp/Log.h>
#include <i2pcpp/util/make_unique.h>
#include <i2pcpp/datatypes/RouterInfo.h>

#include <botan/botan.h>
#include <boost/asio.hpp>

#include <thread>

namespace i2pcpp {
    struct Router::RouterImpl {
        RouterImpl(std::shared_ptr<Database> const &db) :
            work(ios),
            ctx(db, ios),
            log(boost::log::keywords::channel = "R") {}

        boost::asio::io_service ios;
        boost::asio::io_service::work work;
        std::thread serviceThread;

        RouterContext ctx;

        i2p_logger_mt log; ///< Logging object
        bool running;

        TransportPtr trans;
    };

    Router::Router(std::shared_ptr<Database> const &db)
    {
        m_impl = std::make_unique<RouterImpl>(db);
    }

    Router::~Router()
    {
        if(m_impl->serviceThread.joinable())
            m_impl->serviceThread.join();
    }

    void Router::initialize()
    {
        Botan::LibraryInitializer init("thread_safe=true");
    }

    void Router::addTransport(std::shared_ptr<Transport> t)
    {
        t->registerReceivedHandler(boost::bind(
            &InboundMessageDispatcher::messageReceived,
            boost::ref(m_impl->ctx.getInMsgDisp()), _1, _2, _3
        ));
        t->registerEstablishedHandler(boost::bind(
            &InboundMessageDispatcher::connectionEstablished,
            boost::ref(m_impl->ctx.getInMsgDisp()), _1, _2
        ));
        t->registerFailureSignal(boost::bind(
            &InboundMessageDispatcher::connectionFailure,
            boost::ref(m_impl->ctx.getInMsgDisp()), _1
        ));
        t->registerDisconnectedSignal(boost::bind(
            &PeerManager::disconnected, boost::ref(m_impl->ctx.getPeerManager()), _1
        ));

        m_impl->ctx.getOutMsgDisp().registerTransport(t);
    }

    void Router::start()
    {
        I2P_LOG(m_impl->log, info) << "local router hash: " << m_impl->ctx.getIdentity()->getHash();

        m_impl->serviceThread = std::thread([&](){
            while(1) {
                try {
                    m_impl->ios.run();
                    break;
                } catch(std::exception &e) {
                    // TODO Backtrace
                    I2P_LOG(m_impl->log, error) << "exception in service thread: " << e.what();
                }
            }
        });

        /* Peer conected */
        m_impl->ctx.getSignals().registerPeerConnected(boost::bind(
            &PeerManager::connected, boost::ref(m_impl->ctx.getPeerManager()), _1
        ));
        m_impl->ctx.getSignals().registerPeerConnected(boost::bind(
            &OutboundMessageDispatcher::connected,
            boost::ref(m_impl->ctx.getOutMsgDisp()), _1
        ));
        m_impl->ctx.getSignals().registerPeerConnected(boost::bind(
            &DHT::SearchManager::connected,
            boost::ref(m_impl->ctx.getDHT()->getSearchManager()), _1
        ));

        /* Connection failure */
        m_impl->ctx.getSignals().registerConnectionFailure(boost::bind(
            &DHT::SearchManager::connectionFailure,
            boost::ref(m_impl->ctx.getDHT()->getSearchManager()), _1
        ));
        m_impl->ctx.getSignals().registerConnectionFailure(boost::bind(
            &PeerManager::failure, boost::ref(m_impl->ctx.getPeerManager()), _1
        ));

        m_impl->ctx.getSignals().registerSearchReply(boost::bind(
            &DHT::SearchManager::searchReply,
            boost::ref(m_impl->ctx.getDHT()->getSearchManager()), _1, _2, _3
        ));
        m_impl->ctx.getSignals().registerDatabaseStore(boost::bind(
            &DHT::SearchManager::databaseStore,
            boost::ref(m_impl->ctx.getDHT()->getSearchManager()), _1, _2, _3
        ));

        /* Everything related to tunnels */
        m_impl->ctx.getSignals().registerTunnelRecordsReceived(boost::bind(
            &Tunnel::Manager::receiveRecords,
            boost::ref(m_impl->ctx.getTunnelManager()), _1, _2
        ));
        m_impl->ctx.getSignals().registerTunnelGatewayData(boost::bind(
            &Tunnel::Manager::receiveGatewayData,
            boost::ref(m_impl->ctx.getTunnelManager()), _1, _2, _3
        ));
        m_impl->ctx.getSignals().registerTunnelData(boost::bind(
            &Tunnel::Manager::receiveData,
            boost::ref(m_impl->ctx.getTunnelManager()), _1, _2, _3
        ));

        /* Everything related to the DHT */
        m_impl->ctx.getDHT()->getSearchManager().registerSuccess(boost::bind(
            &OutboundMessageDispatcher::dhtSuccess,
            boost::ref(m_impl->ctx.getOutMsgDisp()), _1, _2
        ));
        m_impl->ctx.getDHT()->getSearchManager().registerFailure(
            boost::bind(&OutboundMessageDispatcher::dhtFailure,
            boost::ref(m_impl->ctx.getOutMsgDisp()), _1
        ));

        m_impl->ctx.getPeerManager().begin();
        m_impl->ctx.getTunnelManager().begin();
        m_impl->running = true;
    }

    void Router::stop()
    {
        m_impl->ios.stop();
        m_impl->running = false;
    }

    bool Router::isRunning()
    {
        return m_impl->running;
    }
    
    bool Router::isActive()
    {
        return m_impl->ctx.getTunnelManager().getParticipatingTunnelCount() > 0;
    }

    void Router::gracefulShutdown()
    {
        m_impl->ctx.gracefulShutdown();
        m_impl->trans->gracefulShutdown();
    }
}
