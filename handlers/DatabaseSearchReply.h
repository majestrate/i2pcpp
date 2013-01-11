#ifndef HANDLERSDATABASESEARCHREPLY_H
#define HANDLERSDATABASESEARCHREPLY_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class DatabaseSearchReply : public Message {
			public:
				DatabaseSearchReply(RouterContext &ctx) : Message(ctx) {}

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
		};
	}
}

#endif
