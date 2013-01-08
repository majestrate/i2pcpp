#ifndef HANDLERSDATABASESEARCHREPLY_H
#define HANDLERSDATABASESEARCHREPLY_H

#include "../MessageHandler.h"

namespace i2pcpp {
	namespace Handlers {
		class DatabaseSearchReply : public MessageHandler {
			public:
				DatabaseSearchReply(RouterContext &ctx) : MessageHandler(ctx) {}

				I2NP::Message::Type getType() const;
				JobPtr createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const;
		};
	}
}

#endif
