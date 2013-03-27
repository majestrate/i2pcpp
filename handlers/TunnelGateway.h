#ifndef HANDLERSTUNNELGATEWAY_H
#define HANDLERSTUNNELGATEWAY_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class TunnelGateway : public Message {
			public:
				TunnelGateway(RouterContext &ctx);

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
		};
	}
}

#endif
