#ifndef HANDLERSDATABASESEARCHREPLY_H
#define HANDLERSDATABASESEARCHREPLY_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {
        class DatabaseSearchReply : public Message {
            public:
                DatabaseSearchReply(RouterContext &ctx);

                void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

            private:
                i2p_logger_mt m_log;
        };
    }
}

#endif
