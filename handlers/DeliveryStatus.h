#ifndef HANDLERSDELIVERYSTATUS_H
#define HANDLERSDELIVERYSTATUS_H

#include "../MessageHandler.h"

namespace i2pcpp {
	namespace Handlers {
		class DeliveryStatus : public MessageHandler {
			public:
				DeliveryStatus(RouterContext &ctx) : MessageHandler(ctx) {}

				I2NP::Message::Type getType() const;
				JobPtr createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const;
		};
	}
}

#endif
