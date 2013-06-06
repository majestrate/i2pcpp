#include "Router.h"

/* TEMPORARY */
#include "util/Base64.h"
#include "transport/UDPTransport.h"

namespace i2pcpp {
	Router::Router(std::string const &dbFile) :
		m_work(m_ios),
		m_ctx(dbFile, m_ios),
		m_log(boost::log::keywords::channel = "R")
	{
		I2P_LOG(m_log, info) << "local router hash: " << m_ctx.getIdentity().getHashEncoded();
	}

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
		t->registerEstablishedHandler(boost::bind(&PeerManager::establishmentSuccess, boost::ref(m_ctx.getPeerManager()), _1, _2));
		t->registerFailureSignal(boost::bind(&PeerManager::establishmentFailure, boost::ref(m_ctx.getPeerManager()), _1));
		m_ctx.getOutMsgDisp().registerTransport(t);

		m_ctx.getSignals().registerBuildTunnelRequest(boost::bind(&TunnelManager::handleRequest, boost::ref(m_ctx.getTunnelManager()), _1));

		std::shared_ptr<UDPTransport> u = std::dynamic_pointer_cast<UDPTransport>(t);
		u->start(Endpoint(m_ctx.getDatabase().getConfigValue("ssu_bind_ip"), std::stoi(m_ctx.getDatabase().getConfigValue("ssu_bind_port"))));

		m_ctx.getPeerManager().begin();
	}

	void Router::stop()
	{
		m_ios.stop();
	}

	void Router::connect(std::string const &to)
	{
		RouterInfo ri = m_ctx.getDatabase().getRouterInfo(Base64::decode(to));
		m_ctx.getOutMsgDisp().getTransport()->connect(ri);
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

	void Router::importRouterInfo(ByteArray const &info)
	{
		auto begin = info.cbegin();
		m_ctx.getDatabase().setRouterInfo(RouterInfo(begin, info.cend()));
	}

	void Router::sendRawData(std::string const &dst, std::string const &data)
	{
		ByteArray dataBytes(data.cbegin(), data.cend());

		m_ctx.getOutMsgDisp().getTransport()->send(Base64::decode(dst), dataBytes);
	}

	void Router::createTunnel(bool inbound)
	{
		m_ctx.getTunnelManager().createTunnel(inbound);
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
