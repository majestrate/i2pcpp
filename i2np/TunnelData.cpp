#include "TunnelData.h"

namespace i2pcpp {
	namespace I2NP {
		TunnelData::TunnelData(uint32_t const tunnelId, ByteArray const &data) :
			m_tunnelId(tunnelId)
		{
			std::copy(data.cbegin(), data.cbegin() + 1024, m_data.begin());
		}

		ByteArray TunnelData::getBytes() const
		{
			ByteArray b(4 + 1024);

			b.insert(b.end(), m_tunnelId >> 24);
			b.insert(b.end(), m_tunnelId >> 16);
			b.insert(b.end(), m_tunnelId >> 8);
			b.insert(b.end(), m_tunnelId);

			b.insert(b.end(), m_data.cbegin(), m_data.cend());

			return b;
		}

		bool TunnelData::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			if(end - begin < 4 + 1024)
				return false;

			m_tunnelId = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			std::copy(begin, begin + 1024, m_data.begin());

			return true;
		}
	}
}
