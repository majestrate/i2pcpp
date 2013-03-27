#include "DeliveryStatus.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray DeliveryStatus::getBytes() const
		{
		}

		bool DeliveryStatus::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			m_msgId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			m_timestamp = Date(begin, end);

			return true;
		}
	}
}
