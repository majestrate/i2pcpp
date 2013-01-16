#include "VariableTunnelBuild.h"

#include "../i2np/VariableTunnelBuild.h"
#include "../OutboundMessageDispatcher.h"
#include "../Signals.h"
#include "../util/Base64.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type VariableTunnelBuild::getType() const
		{
			return I2NP::Message::Type::VARIABLE_TUNNEL_BUILD;
		}

		void VariableTunnelBuild::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::VariableTunnelBuild> vtb = std::dynamic_pointer_cast<I2NP::VariableTunnelBuild>(msg);

			std::cerr << "Received VariableTunnelBuild message from " << Base64::encode(ByteArray(from.cbegin(), from.cend())) << "\n";

			m_ctx.getSignals().invokeBuildTunnelRequest(vtb->getRecords());
		}
	}
}
