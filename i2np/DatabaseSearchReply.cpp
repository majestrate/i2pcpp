#include "DatabaseSearchReply.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray DatabaseSearchReply::getBytes() const
		{
			return ByteArray();
		}

		bool DatabaseSearchReply::parse(ByteArray::const_iterator &dataItr)
		{
			copy(dataItr, dataItr + 32, m_key.begin()), dataItr += 32;

			unsigned char size = *(dataItr++);
			while(size--) {
				m_hashes.emplace_back();
				copy(dataItr, dataItr + 32, m_hashes.back().begin()), dataItr += 32;
			}

			copy(dataItr, dataItr + 32, m_from.begin()), dataItr += 32;

			return true;
		}
	}
}
