#include "DatabaseSearchReply.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray DatabaseSearchReply::getBytes() const
		{
			return ByteArray();
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
