#ifndef JOBSDATABASESTORE_H
#define JOBSDATABASESTORE_H

#include "MessageHandlerJob.h"

namespace i2pcpp {
	namespace Jobs {
		class DatabaseStore : public MessageHandlerJob {
			public:
				DatabaseStore(RouterContext &ctx, RouterHash const &from, I2NP::MessagePtr const &msg) : MessageHandlerJob(ctx, from, msg) {}

				void run();
		};
	}
}

#endif
