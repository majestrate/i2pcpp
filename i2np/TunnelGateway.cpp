#include "TunnelGateway.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray TunnelGateway::getBytes() const
		{
			uint16_t size = m_data.size();
			ByteArray b(6 + size);

			b.insert(b.end(), m_tunnelId >> 24);
			b.insert(b.end(), m_tunnelId >> 16);
			b.insert(b.end(), m_tunnelId >> 8);
			b.insert(b.end(), m_tunnelId);

			b.insert(b.end(), size >> 8);
			b.insert(b.end(), size);

			b.insert(b.end(), m_data.cbegin(), m_data.cend());

			return b;
		}

		bool TunnelGateway::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			return true;
		}
	}
}
