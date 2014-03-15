/**
 * @file VariableTunnelBuildReply.cpp
 * @brief Defines the i2pcpp::Handlers::VariableTunnelBuildReply handler.
 */
#ifndef HANDLERSVARIABLETUNNELBUILDREPLY_H
#define HANDLERSVARIABLETUNNELBUILDREPLY_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {

        /**
         * Handles the responses to variable tunnel build messages.
         */
        class VariableTunnelBuildReply : public Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                VariableTunnelBuildReply(RouterContext &ctx);

                /**
                 * Handles the response to the variable tunnel build message by
                 * invoking the appropriate *  signal.
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
