#include "DatabaseStore.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray DatabaseStore::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(), m_key.cbegin(), m_key.cend());
			b.insert(b.end(), m_type);

			b.insert(b.end(), m_replyToken >> 24);
			b.insert(b.end(), m_replyToken >> 16);
			b.insert(b.end(), m_replyToken >> 8);
			b.insert(b.end(), m_replyToken);

			if(m_type == DataType::ROUTER_INFO) {
				uint16_t size = m_data.size();
				b.insert(b.end(), size >> 8);
				b.insert(b.end(), size);
			}

			b.insert(b.end(), m_data.cbegin(), m_data.cend());

			return b;
		}

		bool DatabaseStore::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			copy(begin, begin + 32, m_key.begin());
			begin += 32;

			m_type = (DataType)*(begin++);
			m_replyToken = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);

			if(m_replyToken) {
				m_replyTunnelId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
				copy(begin, begin + 32, m_replyGateway.begin());
				begin += 32;
			}

			uint16_t size = (*(begin++) << 8) | *(begin++);
			m_data = ByteArray(begin, begin + size);

			return true;
		}
	}
}
