#include "DeliveryStatus.h"

namespace i2pcpp {
	namespace I2NP {
		DeliveryStatus::DeliveryStatus() {}

		Message::Type DeliveryStatus::getType() const
		{
			return Message::Type::DELIVERY_STATUS;
		}

		DeliveryStatus::DeliveryStatus(uint32_t msgId, Date timestamp) :
			m_msgId(msgId),
			m_timestamp(timestamp) {}

		ByteArray DeliveryStatus::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(), m_msgId >> 24);
			b.insert(b.end(), m_msgId >> 16);
			b.insert(b.end(), m_msgId >> 8);
			b.insert(b.end(), m_msgId);

			ByteArray ts = m_timestamp.serialize();
			b.insert(b.end(), ts.cbegin(), ts.cend());

			return b;
		}

		bool DeliveryStatus::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			m_msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			m_timestamp = Date(begin, end);

			return true;
		}
	}
}
