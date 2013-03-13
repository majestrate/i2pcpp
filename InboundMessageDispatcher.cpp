#include "InboundMessageDispatcher.h"

#include <boost/bind.hpp>

namespace i2pcpp {
	InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_deliveryStatusHandler(ctx),
		m_dbStoreHandler(ctx),
		m_dbSearchReplyHandler(ctx),
		m_variableTunnelBuildHandler(ctx) {}

	void InboundMessageDispatcher::messageReceived(const RouterHash &from, const ByteArray &data)
	{
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

				default:
					std::cerr << "Router: dropping unhandled message of type " << m->getType() << "\n";
					break;
			}
		}
	}

	void InboundMessageDispatcher::connectionEstablished(const RouterHash &rh)
	{
		std::cerr << "Connection established with " << rh << "\n";
	}
}
