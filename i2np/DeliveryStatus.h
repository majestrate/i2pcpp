#ifndef I2NPDELIVERYSTATUS_H
#define I2NPDELIVERYSTATUS_H

#include "Message.h"

#include "../datatypes/Date.h"

namespace i2pcpp {
	namespace I2NP {
		class DeliveryStatus : public Message {
			public:
				~DeliveryStatus() {}

				Message::MessageType getType() { return Message::MessageType::DELIVERY_STATUS; }
			protected:
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				unsigned long m_msgId;
				Date m_timestamp;
		};
	}
}

#endif
