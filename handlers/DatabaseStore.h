#ifndef HANDLERSDATABASESTORE_H
#define HANDLERSDATABASESTORE_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class DatabaseStore : public Message {
			public:
				DatabaseStore(RouterContext &ctx);

				void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

			private:
				i2p_logger_mt m_log;
		};
	}
}

#endif
