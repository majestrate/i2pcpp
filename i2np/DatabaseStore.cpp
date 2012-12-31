#include "DatabaseStore.h"

namespace i2pcpp {
	namespace I2NP {
		bool DatabaseStore::parse(ByteArray::const_iterator &dataItr)
		{
			copy(dataItr, dataItr + 32, m_key.begin());
			dataItr += 32;

			m_type = (DataType)*(dataItr++);
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

		ByteArray DatabaseStore::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(), m_key.begin(), m_key.end());
			b.insert(b.end(), m_type);

			b.insert(b.end(), m_replyToken << 24);
			b.insert(b.end(), m_replyToken << 16);
			b.insert(b.end(), m_replyToken << 8);
			b.insert(b.end(), m_replyToken);

			b.insert(b.end(), m_data.begin(), m_data.end());

			return b;
		}
	}
}
