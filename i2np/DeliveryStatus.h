#ifndef I2NPDELIVERYSTATUS_H
#define I2NPDELIVERYSTATUS_H

#include "../datatypes/Date.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DeliveryStatus : public Message {
			public:
				~DeliveryStatus() {}

				Message::MessageType getType() { return Message::MessageType::DELIVERY_STATUS; }
			protected:
				bool parse() { return true; }
		};
	}
}

#endif
