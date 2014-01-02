/**
 * @file DeliveryStatus.h    
 * @brief Defines the i2pcpp::Handlers::DeliveryStatus handler.
 */ 
#ifndef HANDLERSDELIVERYSTATUS_H
#define HANDLERSDELIVERYSTATUS_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {

        /**
         * Handles delivery status messages.
         */
        class DeliveryStatus : public Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                DeliveryStatus(RouterContext &ctx);

                /**
                 * Handles the delivery status message by sending a
                 *  DatabaseStore message. 
                 * @param from the sending router
                 * @param msg the actual i2pcpp::I2NP::Message object
                 * @todo Make this invoke a signal instead, so that it is more
                 *  consistent with the other handlers.
                 */
                void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

            private:
                i2p_logger_mt m_log; ///< Logging object
        };
    }
}

#endif
