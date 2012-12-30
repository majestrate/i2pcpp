#ifndef HANDLERSDATABASESTORE_H
#define HANDLERSDATABASESTORE_H

#include "../MessageHandler.h"

namespace i2pcpp {
	namespace Handlers {
		class DatabaseStore : public MessageHandler {
			public:
				DatabaseStore(RouterContext &ctx) : MessageHandler(ctx) {}

				I2NP::Message::Type getType() const;
				JobPtr createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const;
		};
	}
}

#endif
