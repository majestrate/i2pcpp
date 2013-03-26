#ifndef HANDLERSMESSAGE_H
#define HANDLERSMESSAGE_H

#include <memory>

#include "../i2np/Message.h"
#include "../datatypes/RouterHash.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		class Message {
			public:
				Message(RouterContext &ctx) : m_ctx(ctx) {}
				virtual ~Message() {}

				virtual I2NP::Message::Type getType() const = 0;
				virtual void handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg) = 0;

			protected:
				RouterContext& m_ctx;
		};

		typedef std::shared_ptr<Message> MessagePtr;
	}
}

#endif
