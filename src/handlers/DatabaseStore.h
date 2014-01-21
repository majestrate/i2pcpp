/**
 * @file DatabaseStore.h
 * @brief Defines the i2pcpp::Handlers::DatabaseStore handler.
 */
#ifndef HANDLERSDATABASESTORE_H
#define HANDLERSDATABASESTORE_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {
        /**
         * Handles database store messages.
         */
        class DatabaseStore : public Message {
            public:
                /**
                 * Constructs from a reference to the i2pcpp::RouterContext
                 *  object.
                 */
                DatabaseStore(RouterContext &ctx);

                /**
                 * Handles the database store message.
                 * If the data to be stored is a (gzip) commpressed i2pcpp::RouterInfo
                 *  object, it is decommpressed and, if the signature if correct,
                 *  it is added to the i2pcpp::Database of the router.
                 * If the data is an uncompressed i2pcpp::LeaseSet, the associated
                 *  signal is invoked (currently unimplemented).
                 * @param from the sending router
                 * @param msg the actual i2pcpp::I2NP::Message object
                 * @toto implement lease set handling (when floodfill
                 *  is implemented)
                 */
                void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

            private:
                i2p_logger_mt m_log; ///< Logging object
        };
    }
}

#endif
