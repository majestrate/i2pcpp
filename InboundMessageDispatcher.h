#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <mutex>
#include <map>

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

#include "JobQueue.h"
#include "MessageHandler.h"

namespace i2pcpp {
	class InboundMessageDispatcher {
		public:
			InboundMessageDispatcher(JobQueue &jq) : m_jobQueue(jq) {}

			void addMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
			void registerHandler(I2NP::Message::Type const mtype, MessageHandlerPtr const &handler);
			void unregisterHandler(I2NP::Message::Type const mtype);

		private:
			JobQueue& m_jobQueue;
			std::map<I2NP::Message::Type, MessageHandlerPtr> m_msgHandlers;

			std::mutex m_mutex;
	};
}

#endif
