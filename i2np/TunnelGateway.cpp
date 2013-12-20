#include "TunnelGateway.h"
#include <stdexcept>

namespace i2pcpp {
	namespace I2NP {
		TunnelGateway::TunnelGateway(uint32_t const tunnelId, ByteArray const &data) :
			m_tunnelId(tunnelId),
			m_data(data) {}

		uint32_t TunnelGateway::getTunnelId() const
		{
			return m_tunnelId;
		}

		const ByteArray& TunnelGateway::getData() const
		{
			return m_data;
		}

		ByteArray TunnelGateway::compile() const
		{
			uint16_t size = m_data.size();
			ByteArray b;

			b.insert(b.end(), m_tunnelId >> 24);
			b.insert(b.end(), m_tunnelId >> 16);
			b.insert(b.end(), m_tunnelId >> 8);
			b.insert(b.end(), m_tunnelId);

			b.insert(b.end(), size >> 8);
			b.insert(b.end(), size);

			b.insert(b.end(), m_data.cbegin(), m_data.cend());

			return b;
		}

		TunnelGateway TunnelGateway::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			TunnelGateway tg;

			tg.m_tunnelId = (begin[0] << 24) | (begin[1] << 16) | (begin[2] << 8) | (begin[3]);
			begin += 4;

			uint16_t size = (begin[0] << 8) | (begin[1]);
			begin += 2;
			if(size > (end - begin))
				throw std::runtime_error("invalid tunnel gateway message");

			tg.m_data = ByteArray(begin, begin + size);

			return tg;
		}
	}
}
