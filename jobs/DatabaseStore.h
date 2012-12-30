#ifndef JOBSDATABASESTORE_H
#define JOBSDATABASESTORE_H

#include "MessageHandlerJob.h"

namespace i2pcpp {
	namespace Jobs {
		class DatabaseStore : public MessageHandlerJob {
			public:
				DatabaseStore(RouterHash const &from, I2NP::MessagePtr const &msg) : MessageHandlerJob(from, msg) {}

				void run();
		};
	}
}

#endif
