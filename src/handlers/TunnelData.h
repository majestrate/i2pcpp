/**
 * @file TunnelData.h
 * @brief Defines the i2pcpp::Handlers::TunnelData handler.
 */
#ifndef HANDLERSTUNNELDATA_H
#define HANDLERSTUNNELDATA_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {

        /**
         * Handles tunnel data messages.
         */
        class TunnelData : public Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                TunnelData(RouterContext &ctx);

                /**
                 * Handles the tunnel data message by invoking the appropriate
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
