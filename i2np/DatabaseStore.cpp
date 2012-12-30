#include "DatabaseStore.h"

namespace i2pcpp {
	namespace I2NP {
		bool DatabaseStore::parse(ByteArray::const_iterator &dataItr)
		{
			copy(dataItr, dataItr + 32, m_key.begin());
			dataItr += 32;

			m_type = (Type)*(dataItr++);
			m_replyToken = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

			if(m_replyToken) {
				m_replyTunnelId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
				copy(dataItr, dataItr + 32, m_replyGateway.begin());
				dataItr += 32;
			}

			unsigned short size = (*(dataItr++) << 8) | *(dataItr++);
			m_data = ByteArray(dataItr, dataItr + size);

			return true;
		}
	}
}
