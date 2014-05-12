/**
 * @file TunnelGateway.cpp
 * @brief Implements TunnelGateway.h
 */
#include "TunnelGateway.h"

#include "../RouterContext.h"

#include "../i2np/TunnelGateway.h"

namespace i2pcpp {
    namespace Handlers {
        TunnelGateway::TunnelGateway(RouterContext &ctx) :
            Message(ctx),
            m_log(I2P_LOG_CHANNEL("H[TG]")) {}

        void TunnelGateway::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
        {
            std::shared_ptr<I2NP::TunnelGateway> tg = std::dynamic_pointer_cast<I2NP::TunnelGateway>(msg);

            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received TunnelGateway message";

            m_ctx.getSignals().invokeTunnelGatewayData(from, tg->getTunnelId(), tg->getData());
        }
    }
}
