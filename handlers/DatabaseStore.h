#ifndef HANDLERSDATABASESTORE_H
#define HANDLERSDATABASESTORE_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class DatabaseStore : public Message {
			public:
				DatabaseStore(RouterContext &ctx) : Message(ctx) {}

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
		};
	}
}

#endif
