#include "TunnelGateway.h"

#include "../i2np/TunnelGateway.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		TunnelGateway::TunnelGateway(RouterContext &ctx) :
			Message(ctx) {}

		I2NP::Message::Type TunnelGateway::getType() const
		{
			return I2NP::Message::Type::TUNNEL_GATEWAY;
		}

		void TunnelGateway::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::TunnelGateway> tg = std::dynamic_pointer_cast<I2NP::TunnelGateway>(msg);

			I2P_LOG_RH(m_ctx.getLogger(), from);
			BOOST_LOG_SEV(m_ctx.getLogger(), debug) << "received TunnelGateway message";
		}
	}
}
