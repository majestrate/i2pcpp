#include "InboundMessageDispatcher.h"

#include <boost/bind.hpp>

#include <botan/auto_rng.h>

#include "i2np/DeliveryStatus.h"

#include "RouterContext.h"

namespace i2pcpp {
	InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_deliveryStatusHandler(ctx),
		m_dbStoreHandler(ctx),
		m_dbSearchReplyHandler(ctx),
		m_variableTunnelBuildHandler(ctx),
		m_log(boost::log::keywords::channel = "IMD") {}

	void InboundMessageDispatcher::messageReceived(RouterHash from, ByteArray data)
	{
		I2P_LOG_RH(m_log, from);

		std::stringstream s;
		s << std::setw(2) << std::setfill('0') << std::hex;
		for(auto c: data) s << (int)c;
		BOOST_LOG_SEV(m_log, debug) << "received data: " << s.str();

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

				case I2NP::Message::Type::TUNNEL_GATEWAY:
					break;

				case I2NP::Message::Type::VARIABLE_TUNNEL_BUILD:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_variableTunnelBuildHandler, from, m));
					break;

				default:
					BOOST_LOG_SEV(m_log, error) << "dropping unhandled message of type " << m->getType();
					break;
			}
		}
	}

	void InboundMessageDispatcher::connectionEstablished(RouterHash rh, bool inbound)
	{
		I2P_LOG_RH(m_log, rh);
		BOOST_LOG_SEV(m_log, info) << "session established";

		if(inbound) {
			Botan::AutoSeeded_RNG rng;
			uint32_t msgId;
			rng.randomize((unsigned char *)&msgId, sizeof(msgId));

			I2NP::MessagePtr m(new I2NP::DeliveryStatus(msgId, Date(2)));
			m_ctx.getOutMsgDisp().sendMessage(rh, m);
		}
	}
}
