#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include "JobQueue.h"

#include "i2np/Message.h"

namespace i2pcpp {
	class InboundMessageDispatcher {
		public:
			InboundMessageDispatcher(JobQueue &jq) : m_jobQueue(jq) {}

			void addMessage(I2NP::MessagePtr const &msg) {}

		private:
			JobQueue& m_jobQueue;
	};
}

#endif
