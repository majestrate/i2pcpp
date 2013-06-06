#ifndef HANDLERSDELIVERYSTATUS_H
#define HANDLERSDELIVERYSTATUS_H

#include "Message.h"

namespace i2pcpp {
	namespace Handlers {
		class DeliveryStatus : public Message {
			public:
				DeliveryStatus(RouterContext &ctx);

				I2NP::Message::Type getType() const;
				void handleMessage(RouterHash const from, I2NP::MessagePtr const msg);

			private:
				i2p_logger_mt m_log;
		};
	}
}

#endif
