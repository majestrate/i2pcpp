#include "Message.h"

#include "DeliveryStatus.h"

#include <iostream>

namespace i2pcpp {
	namespace I2NP {
		Message *Message::fromBytes(ByteArray const &data)
		{
			Message *m;

			auto dataItr = data.cbegin();
			MessageType mtype = (MessageType)*(dataItr++);

			switch(mtype)
			{
				case MessageType::DELIVERY_STATUS:
					m = new DeliveryStatus();
					break;

				default:
					return NULL;
			}

			m->m_expiration = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

			if(m->parse(dataItr))
				return m;
			else
				return NULL;
		}
	}
}
