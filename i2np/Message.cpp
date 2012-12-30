#include "Message.h"

#include "DeliveryStatus.h"
#include "DatabaseStore.h"

#include <iostream>

namespace i2pcpp {
	namespace I2NP {
		MessagePtr Message::fromBytes(ByteArray const &data)
		{
			MessagePtr m;

			auto dataItr = data.cbegin();
			Type mtype = (Type)*(dataItr++);

			switch(mtype)
			{
				case Type::DELIVERY_STATUS:
					m = MessagePtr(new DeliveryStatus());
					break;

				case Type::DB_STORE:
					m = MessagePtr(new DatabaseStore());
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
