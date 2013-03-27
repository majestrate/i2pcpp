#include "VariableTunnelBuild.h"

#include "../i2np/VariableTunnelBuild.h"
#include "../Signals.h"
#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		VariableTunnelBuild::VariableTunnelBuild(RouterContext &ctx) :
			Message(ctx) {}

		I2NP::Message::Type VariableTunnelBuild::getType() const
		{
			return I2NP::Message::Type::VARIABLE_TUNNEL_BUILD;
		}

		void VariableTunnelBuild::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::VariableTunnelBuild> vtb = std::dynamic_pointer_cast<I2NP::VariableTunnelBuild>(msg);

			I2P_LOG_RH(m_ctx.getLogger(), from);
			BOOST_LOG_SEV(m_ctx.getLogger(), debug) << "received VariableTunnelBuild message";

			m_ctx.getSignals().invokeBuildTunnelRequest(vtb->getRecords());
		}
	}
}
