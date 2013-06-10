#include "Router.h"

/* TEMPORARY */
#include "util/Base64.h"
#include "transport/UDPTransport.h"

namespace i2pcpp {
	Router::Router(std::string const &dbFile) :
		m_work(m_ios),
		m_ctx(dbFile, m_ios),
		m_log(boost::log::keywords::channel = "R") {}

	Router::~Router()
	{
		if(m_serviceThread.joinable()) m_serviceThread.join();
	}
      
        i2p_logger_mt & Router::getLogger()
        {
	  return m_log;
        }


	void Router::start()
	{
		I2P_LOG(m_log, info) << "local router hash: " << m_ctx.getIdentity().getHashEncoded();

		m_serviceThread = std::thread([&](){
			while(1) {
				try {
					m_ios.run();
					break;
				} catch(std::exception &e) {
					// TODO Handle exception
					I2P_LOG(m_log, error) << "exception in service thread: " << e.what();
				}
			}
		});
		TransportPtr t = TransportPtr(new UDPTransport(*m_ctx.getSigningKey(), m_ctx.getIdentity()));
		t->registerReceivedHandler(boost::bind(&InboundMessageDispatcher::messageReceived, m_ctx.getInMsgDisp(), _1, _2));
		t->registerEstablishedHandler(boost::bind(&InboundMessageDispatcher::connectionEstablished, m_ctx.getInMsgDisp(), _1, _2));
		t->registerFailureSignal(boost::bind(&PeerManager::failure, boost::ref(m_ctx.getPeerManager()), _1));
		t->registerDisconnectedSignal(boost::bind(&PeerManager::disconnected, boost::ref(m_ctx.getPeerManager()), _1));
		m_ctx.getOutMsgDisp().registerTransport(t);

		m_ctx.getSignals().registerTunnelRecordsReceived(boost::bind(&TunnelManager::receiveRecords, boost::ref(m_ctx.getTunnelManager()), _1));
		m_ctx.getSignals().registerRouterInfoSaved(boost::bind(&OutboundMessageDispatcher::infoSaved, boost::ref(m_ctx.getOutMsgDisp()), _1));
		m_ctx.getSignals().registerPeerConnected(boost::bind(&PeerManager::connected, boost::ref(m_ctx.getPeerManager()), _1));
		m_ctx.getSignals().registerPeerConnected(boost::bind(&OutboundMessageDispatcher::connected, boost::ref(m_ctx.getOutMsgDisp()), _1));

		std::shared_ptr<UDPTransport> u = std::static_pointer_cast<UDPTransport>(t);
		u->start(Endpoint(m_ctx.getDatabase().getConfigValue("ssu_bind_ip"), std::stoi(m_ctx.getDatabase().getConfigValue("ssu_bind_port"))));

		m_ctx.getPeerManager().begin();
	}

	void Router::stop()
	{
		m_ios.stop();
	}

	ByteArray Router::getRouterInfo()
	{
		Mapping am;
		am.setValue("caps", "BC");
		am.setValue("host", m_ctx.getDatabase().getConfigValue("ssu_external_ip"));
		am.setValue("key", m_ctx.getIdentity().getHashEncoded());
		am.setValue("port", m_ctx.getDatabase().getConfigValue("ssu_external_port"));
		RouterAddress a(5, Date(0), "SSU", am);

		Mapping rm;
		rm.setValue("coreVersion", "0.9.5");
		rm.setValue("netId", "2");
		rm.setValue("router.version", "0.9.5");
		rm.setValue("stat_uptime", "90m");
		rm.setValue("caps", "MR");
		RouterInfo myInfo(m_ctx.getIdentity(), Date(), rm);
		myInfo.addAddress(a);
		myInfo.sign(m_ctx.getSigningKey());

		return myInfo.serialize();
	}

	void Router::importRouter(RouterInfo const &router)
	{
		m_ctx.getDatabase().setRouterInfo(router);
	}

	void Router::importRouter(std::vector<RouterInfo> const &routers)
	{
		m_ctx.getDatabase().setRouterInfo(routers);
	}

	void Router::deleteAllRouters()
	{
		m_ctx.getDatabase().deleteAllRouters();
	}

	void Router::setConfigValue(std::string key, std::string value)
	{
		m_ctx.getDatabase().setConfigValue(key, value);
	}

	std::string Router::getConfigValue(std::string key)
	{
		return m_ctx.getDatabase().getConfigValue(key);
	}
       
  void Router::importNetDb(std::string const & dirname)
  {
    BOOST_LOG_SEV(getLogger(), info) << "Load netdb from " << dirname;
      
    if(m_ctx.getDatabase().importNetDb(dirname))
      {
	BOOST_LOG_SEV(getLogger(), info) << "Loaded Netdb fine";
      }
    else
      {
	BOOST_LOG_SEV(getLogger(), warning) << "Failed to load netdb from " << dirname;
      
      }
  }
}
