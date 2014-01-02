/**
 * @file DatabaseSearchReply.h
 * @brief Defines the i2pcpp::Handlers::DatabaseSearchReply handler.
 */
#ifndef HANDLERSDATABASESEARCHREPLY_H
#define HANDLERSDATABASESEARCHREPLY_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {

        /**
         * Handles reponses to database search messages. 
         */
        class DatabaseSearchReply : public Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                DatabaseSearchReply(RouterContext &ctx);

                /**
                 * Handles the database search response message by invoking the appropriate
                 *  signal.
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
