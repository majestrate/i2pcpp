#ifndef HANDLERSVARIABLETUNNELBUILD_H
#define HANDLERSVARIABLETUNNELBUILD_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class VariableTunnelBuild : public Message {
			public:
				VariableTunnelBuild(RouterContext &ctx);

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
		};
	}
}

#endif
