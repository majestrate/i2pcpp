#include "Message.h"

#include "DeliveryStatus.h"
#include "DatabaseStore.h"

#include <iostream>
#include <iomanip>

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

		ByteArray Message::toBytes() const
		{
			ByteArray b;
			const ByteArray& m = getBytes();

			b.insert(b.end(), getType());

			// m_expiration?
			uint32_t expiration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 60;
			b.insert(b.end(), expiration >> 24);
			b.insert(b.end(), expiration >> 16);
			b.insert(b.end(), expiration >> 8);
			b.insert(b.end(), expiration);

			b.insert(b.end(), m.cbegin(), m.cend());

			return b;
		}
	}
}
