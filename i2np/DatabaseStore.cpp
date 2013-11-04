#include "DatabaseStore.h"

namespace i2pcpp {
	namespace I2NP {
		DatabaseStore::DatabaseStore() {}

		DatabaseStore::DatabaseStore(StaticByteArray<32> const &key, DataType type, uint32_t replyToken, ByteArray const &data) :
			Message(),
			m_key(key),
			m_type(type),
			m_replyToken(replyToken),
			m_data(data) {}

		Message::Type DatabaseStore::getType() const
		{
			return Message::Type::DB_STORE;
		}

		DatabaseStore::DataType DatabaseStore::getDataType() const
		{
			return m_type;
		}

		uint32_t DatabaseStore::getReplyToken() const
		{
			return m_replyToken;
		}

		const ByteArray& DatabaseStore::getData() const
		{
			return m_data;
		}

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
