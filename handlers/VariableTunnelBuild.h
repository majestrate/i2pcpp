#ifndef HANDLERSVARIABLETUNNELBUILD_H
#define HANDLERSVARIABLETUNNELBUILD_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class VariableTunnelBuild : public Message {
			public:
				VariableTunnelBuild(RouterContext &ctx);

				void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

			private:
				i2p_logger_mt m_log;
		};
	}
}

#endif
