#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <memory>

#include "i2np/Message.h"
#include "datatypes/RouterHash.h"

#include "Job.h"
#include "RouterContext.h"

namespace i2pcpp {
	class MessageHandler {
		public:
			MessageHandler(RouterContext &ctx) : m_ctx(ctx) {}

			virtual I2NP::Message::Type getType() const = 0;
			virtual JobPtr createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const = 0;

		protected:
			RouterContext& m_ctx;
	};

	typedef std::shared_ptr<MessageHandler> MessageHandlerPtr;
}

#endif
