#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <memory>

#include "i2np/Message.h"
#include "datatypes/RouterHash.h"

#include "Job.h"

namespace i2pcpp {
	class MessageHandler {
		public:
			virtual I2NP::Message::Type getType() const = 0;
			virtual JobPtr createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const = 0;
	};

	typedef std::shared_ptr<MessageHandler> MessageHandlerPtr;
}

#endif
