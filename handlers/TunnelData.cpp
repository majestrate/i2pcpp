/**
 * @file TunnelData.cpp
 * @brief Implements TunnelData.h
 */
#include "TunnelData.h"

#include "../i2np/TunnelData.h"

#include "../RouterContext.h"

namespace i2pcpp {
    namespace Handlers {
        TunnelData::TunnelData(RouterContext &ctx) :
            Message(ctx),
            m_log(boost::log::keywords::channel = "H[TD]") {}

        void TunnelData::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
        {
            std::shared_ptr<I2NP::TunnelData> td = std::dynamic_pointer_cast<I2NP::TunnelData>(msg);

            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received TunnelData message";

            m_ctx.getSignals().invokeTunnelData(from, td->getTunnelId(), td->getData());
        }
    }
}
