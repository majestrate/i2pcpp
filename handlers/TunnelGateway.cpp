#include "TunnelGateway.h"

#include "../i2np/TunnelGateway.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		TunnelGateway::TunnelGateway(RouterContext &ctx) :
			Message(ctx),
			m_log(boost::log::keywords::channel = "H[TG]") {}

		I2NP::Message::Type TunnelGateway::getType() const
		{
			return I2NP::Message::Type::TUNNEL_GATEWAY;
		}

		void TunnelGateway::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
		{
			std::shared_ptr<I2NP::TunnelGateway> tg = std::dynamic_pointer_cast<I2NP::TunnelGateway>(msg);

			I2P_LOG_SCOPED_RH(m_log, from);
			I2P_LOG(m_log, debug) << "received TunnelGateway message";

			m_ctx.getSignals().invokeTunnelGatewayData(tg->getTunnelId(), tg->getData());
		}
	}
}
