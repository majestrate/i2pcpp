#ifndef JOBSMESSAGEHANDLERJOB_H
#define JOBSMESSAGEHANDLERJOB_H

#include "../Job.h"
#include "../datatypes/RouterHash.h"
#include "../i2np/Message.h"

namespace i2pcpp {
	namespace Jobs {
		class MessageHandlerJob : public Job {
			public:
				MessageHandlerJob(RouterHash const &from, I2NP::MessagePtr const &msg) : m_from(from), m_msg(msg) {}

			protected:
				RouterHash m_from;
				I2NP::MessagePtr m_msg;
		};
	}
}

#endif
