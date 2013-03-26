#ifndef I2NPDELIVERYSTATUS_H
#define I2NPDELIVERYSTATUS_H

#include "Message.h"

#include "../datatypes/Date.h"

namespace i2pcpp {
	namespace I2NP {
		class DeliveryStatus : public Message {
			public:
				Message::Type getType() const { return Message::Type::DELIVERY_STATUS; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				uint32_t m_msgId;
				Date m_timestamp;
		};
	}
}

#endif
