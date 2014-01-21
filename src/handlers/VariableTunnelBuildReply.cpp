/**
 * @file VariableTunnelBuildReply.cpp
 * @brief Implements VariableTunnelBuildReply.h
 */
#include "VariableTunnelBuildReply.h"

#include "../i2np/VariableTunnelBuildReply.h"
#include "../Signals.h"
#include "../RouterContext.h"

namespace i2pcpp {
    namespace Handlers {
        VariableTunnelBuildReply::VariableTunnelBuildReply(RouterContext &ctx) :
            Message(ctx),
            m_log(I2P_LOG_CHANNEL("H[VTBR]")) {}

        void VariableTunnelBuildReply::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
        {
            std::shared_ptr<I2NP::VariableTunnelBuildReply> vtbr = std::dynamic_pointer_cast<I2NP::VariableTunnelBuildReply>(msg);

            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received VariableTunnelBuildReply message";

            m_ctx.getSignals().invokeTunnelRecordsReceived(vtbr->getMsgId(), vtbr->getRecords());
        }
    }
}
