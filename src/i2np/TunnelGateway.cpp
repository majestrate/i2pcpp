/**
 * @file TunnelGateway.cpp
 * @brief Implements TunnelGateway.h
 */
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

            tg.m_tunnelId = parseUint32(begin);

            uint16_t size = parseUint16(begin);
            if(size > (end - begin))
                throw std::runtime_error("invalid tunnel gateway message");

            tg.m_data = ByteArray(begin, begin + size);

            return tg;
        }
    }
}
