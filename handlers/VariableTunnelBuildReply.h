#ifndef HANDLERSVARIABLETUNNELBUILDREPLY_H
#define HANDLERSVARIABLETUNNELBUILDREPLY_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class VariableTunnelBuildReply : public Message {
			public:
				VariableTunnelBuildReply(RouterContext &ctx);

				void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

			private:
				i2p_logger_mt m_log;
		};
	}
}

#endif
