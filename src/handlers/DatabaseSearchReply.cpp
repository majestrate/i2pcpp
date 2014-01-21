/**
 * @file DatabaseSearchReply.cpp
 * @brief Implements DatabaseSearchReply.h
 */
#include "DatabaseSearchReply.h"

#include "../i2np/DatabaseSearchReply.h"
#include "../i2np/DatabaseLookup.h"

#include "../RouterContext.h"

namespace i2pcpp {
    namespace Handlers {
        DatabaseSearchReply::DatabaseSearchReply(RouterContext &ctx) :
            Message(ctx),
            m_log(I2P_LOG_CHANNEL("H[DSR]")) {}

        void DatabaseSearchReply::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
        {
            std::shared_ptr<I2NP::DatabaseSearchReply> dsr = std::dynamic_pointer_cast<I2NP::DatabaseSearchReply>(msg);

            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received DatabaseSearchReply message";

            m_ctx.getSignals().invokeSearchReply(from, dsr->getKey(), dsr->getHashes());
        }
    }
}
