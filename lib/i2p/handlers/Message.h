/**
 * @file Message.h
 * @brief Defines the i2pcpp::Handler::Message handler base class.
 */
#ifndef HANDLERSMESSAGE_H
#define HANDLERSMESSAGE_H

#include <memory>

#include "../i2np/Message.h"
#include <i2pcpp/datatypes/RouterHash.h>
#include "../Log.h"

namespace i2pcpp {
    class RouterContext;

    namespace Handlers {

        /**
         * Abstract base class for i2pcpp::I2NP::Message object handlers.
         */
        class Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                Message(RouterContext &ctx);
                virtual ~Message() {}

                /**
                 * Pure virtual handler function.
                 * @param from the sending router
                 * @param msg the actual i2pcpp::I2NP::Message object
                 */
                virtual void handleMessage(RouterHash const from, I2NP::MessagePtr const msg) = 0;

            protected:
                /// A reference to the i2pcpp::RouterContext object
                RouterContext& m_ctx;
        };

        typedef std::shared_ptr<Message> MessagePtr;
    }
}

#endif
