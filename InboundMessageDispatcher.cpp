#include "InboundMessageDispatcher.h"

#include <iomanip>

#include <boost/bind.hpp>
#include <iomanip>
#include <botan/auto_rng.h>
#include <botan/zlib.h>

#include "i2np/DeliveryStatus.h"
#include "i2np/DatabaseStore.h"

#include "RouterContext.h"

namespace i2pcpp {
	InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_deliveryStatusHandler(ctx),
		m_dbStoreHandler(ctx),
		m_dbSearchReplyHandler(ctx),
		m_variableTunnelBuildHandler(ctx),
		m_tunnelGatewayHandler(ctx),
		m_log(boost::log::keywords::channel = "IMD") {}

	void InboundMessageDispatcher::messageReceived(RouterHash const from, ByteArray data)
	{
		I2P_LOG_SCOPED_RH(m_log, from);

		std::stringstream s;
		for(auto c: data) s << std::setw(2) << std::setfill('0') << std::hex << (int)c;
		I2P_LOG(m_log, debug) << "received data: " << s.str();

		I2NP::MessagePtr m = I2NP::Message::fromBytes(data);
		if(m) {
			switch(m->getType())
			{
				case I2NP::Message::Type::DELIVERY_STATUS:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_deliveryStatusHandler, from, m));
					break;

				case I2NP::Message::Type::DB_STORE:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbStoreHandler, from, m));
					break;

				case I2NP::Message::Type::DB_SEARCH_REPLY:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbSearchReplyHandler, from, m));
					break;

				case I2NP::Message::Type::VARIABLE_TUNNEL_BUILD:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_variableTunnelBuildHandler, from, m));
					break;

				case I2NP::Message::Type::TUNNEL_GATEWAY:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_tunnelGatewayHandler, from, m));
					break;

				default:
					I2P_LOG(m_log, error) << "dropping unhandled message of type " << m->getType();
					break;
			}
		}
	}

	void InboundMessageDispatcher::connectionEstablished(RouterHash const rh, bool inbound)
	{
		I2P_LOG_SCOPED_RH(m_log, rh);
		I2P_LOG(m_log, info) << "session established";

		if(inbound) {
			Botan::AutoSeeded_RNG rng;
			uint32_t msgId;
			rng.randomize((unsigned char *)&msgId, sizeof(msgId));

			I2NP::MessagePtr m(new I2NP::DeliveryStatus(msgId, Date(2)));
			m_ctx.getOutMsgDisp().sendMessage(rh, m);

			Mapping am;
			am.setValue("caps", "BC");
			am.setValue("host", m_ctx.getDatabase().getConfigValue("ssu_external_ip"));
			am.setValue("key", m_ctx.getIdentity().getHashEncoded());
			am.setValue("port", m_ctx.getDatabase().getConfigValue("ssu_external_port"));
			RouterAddress a(5, Date(0), "SSU", am);

			Mapping rm;
			rm.setValue("coreVersion", "0.9.6");
			rm.setValue("netId", "2");
			rm.setValue("router.version", "0.9.6");
			rm.setValue("stat_uptime", "90m");
			rm.setValue("caps", "OR");
			RouterInfo myInfo(m_ctx.getIdentity(), Date(), rm);
			myInfo.addAddress(a);
			myInfo.sign(m_ctx.getSigningKey());

			Botan::Pipe gzPipe(new Botan::Zlib_Compression);
			gzPipe.start_msg();

			gzPipe.write(myInfo.serialize());
			
			gzPipe.end_msg();

			unsigned int size = gzPipe.remaining();
			ByteArray gzInfoBytes(size);
			gzPipe.read(gzInfoBytes.data(), size);

			auto mydsm = std::make_shared<I2NP::DatabaseStore>(myInfo.getIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes);
			m_ctx.getOutMsgDisp().sendMessage(rh, mydsm);
		}

		m_ctx.getSignals().invokePeerConnected(rh);
	}

	void InboundMessageDispatcher::connectionFailure(RouterHash const rh)
	{
		m_ctx.getSignals().invokeConnectionFailure(rh);
	}
}
