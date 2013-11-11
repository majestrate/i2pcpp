#include "DatabaseSearchReply.h"

namespace i2pcpp {
	namespace I2NP {
		DatabaseSearchReply::DatabaseSearchReply() {}

		Message::Type DatabaseSearchReply::getType() const
		{
			return Message::Type::DB_SEARCH_REPLY;
		}

		ByteArray DatabaseSearchReply::getBytes() const
		{
			// TODO
			return ByteArray();
		}

		const std::array<unsigned char, 32>& DatabaseSearchReply::getKey() const
		{
			return m_key;
		}

		const std::list<RouterHash>& DatabaseSearchReply::getHashes() const
		{
			return m_hashes;
		}

		const RouterHash& DatabaseSearchReply::getFrom() const
		{
			return m_from;
		}

		bool DatabaseSearchReply::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			copy(begin, begin + 32, m_key.begin()), begin += 32;

			unsigned char size = *(begin++);
			while(size--) {
				m_hashes.emplace_back();
				copy(begin, begin + 32, m_hashes.back().begin()), begin += 32;
			}

			copy(begin, begin + 32, m_from.begin()), begin += 32;

			return true;
		}
	}
}
