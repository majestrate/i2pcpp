/**
 * @file VariableTunnelBuild.h
 * @brief Defines the i2pcpp::Handlers::VariableTunnelBuild handler.
 */
#ifndef HANDLERSVARIABLETUNNELBUILD_H
#define HANDLERSVARIABLETUNNELBUILD_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {
        /**
         * Handles variable tunnel build messages.
         */
        class VariableTunnelBuild : public Message {
            public:

                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                VariableTunnelBuild(RouterContext &ctx);

                /**
                 * Handles the variable tunnel tunnel build message by invoking
                 * the appropriate signal.
                 * @param from the sending router
                 * @param msg the actual i2pcpp::I2NP::Message object
                 */
                void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

            private:
                i2p_logger_mt m_log; ///< Logging object
        };
    }
}

#endif
