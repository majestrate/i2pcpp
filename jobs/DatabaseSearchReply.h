#ifndef JOBSDATABASESEARCHREPLY_H
#define JOBSDATABASESEARCHREPLY_H

#include "MessageHandlerJob.h"

namespace i2pcpp {
	namespace Jobs {
		class DatabaseSearchReply : public MessageHandlerJob {
			public:
				DatabaseSearchReply(RouterContext &ctx, RouterHash const &from, I2NP::MessagePtr const &msg) : MessageHandlerJob(ctx, from, msg) {}

				void run();
		};
	}
}

#endif
