#include "TunnelGateway.h"

#include "../i2np/TunnelGateway.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type TunnelGateway::getType() const
		{
			return I2NP::Message::Type::TUNNEL_GATEWAY;
		}

		void TunnelGateway::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::TunnelGateway> tg = std::dynamic_pointer_cast<I2NP::TunnelGateway>(msg);

			//std::cerr << "Received TunnelGateway message from " << from << " for tunnel " << tg->getTunnelId() << "\n";
		}
	}
}
