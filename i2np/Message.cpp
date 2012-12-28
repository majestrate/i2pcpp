#include "Message.h"

#include "DeliveryStatus.h"

#include <iostream>

namespace i2pcpp {
	namespace I2NP {
		MessagePtr Message::fromBytes(ByteArray const &data)
		{
			MessagePtr m;

			auto dataItr = data.cbegin();
			MessageType mtype = (MessageType)*(dataItr++);

			switch(mtype)
			{
				case MessageType::DELIVERY_STATUS:
					m = MessagePtr(new DeliveryStatus());
					break;

				default:
					return MessagePtr();
			}

			m->m_expiration = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

			if(m->parse(dataItr))
				return m;
			else
				return MessagePtr();
		}
	}
}
