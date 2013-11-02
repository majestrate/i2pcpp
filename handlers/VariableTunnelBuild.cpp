#include "VariableTunnelBuild.h"

#include "../i2np/VariableTunnelBuild.h"
#include "../Signals.h"
#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		VariableTunnelBuild::VariableTunnelBuild(RouterContext &ctx) :
			Message(ctx),
			m_log(boost::log::keywords::channel = "H[VTB]") {}

		I2NP::Message::Type VariableTunnelBuild::getType() const
		{
			return I2NP::Message::Type::VARIABLE_TUNNEL_BUILD;
		}

		void VariableTunnelBuild::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
		{
			std::shared_ptr<I2NP::VariableTunnelBuild> vtb = std::dynamic_pointer_cast<I2NP::VariableTunnelBuild>(msg);

			I2P_LOG_SCOPED_RH(m_log, from);
			I2P_LOG(m_log, debug) << "received VariableTunnelBuild message";

			m_ctx.getSignals().invokeTunnelRecordsReceived(vtb->getMsgId(), vtb->getRecords());
		}
	}
}
