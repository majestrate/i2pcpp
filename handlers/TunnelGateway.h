#ifndef HANDLERSTUNNELGATEWAY_H
#define HANDLERSTUNNELGATEWAY_H

#include "Message.h"

namespace i2pcpp {
    namespace Handlers {
        class TunnelGateway : public Message {
            public:
                TunnelGateway(RouterContext &ctx);

                void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

            private:
                i2p_logger_mt m_log;
        };
    }
}

#endif
