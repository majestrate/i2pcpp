#ifndef HANDLERSDELIVERYSTATUS_H
#define HANDLERSDELIVERYSTATUS_H

#include "../MessageHandler.h"

namespace i2pcpp {
	namespace Handlers {
		class DeliveryStatus : public MessageHandler {
			public:
				I2NP::Message::MessageType getType() const;
				JobPtr createJob() const;

			private:
		};
	}
}

#endif
