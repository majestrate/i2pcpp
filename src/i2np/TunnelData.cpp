/**
 * @file TunnelData.cpp
 * @brief Implements TunnelData.h
 */
#include "TunnelData.h"

namespace i2pcpp {
    namespace I2NP {
        TunnelData::TunnelData(uint32_t const tunnelId, StaticByteArray<1024> const &data) :
            m_tunnelId(tunnelId),
            m_data(data) {}

        uint32_t TunnelData::getTunnelId() const
        {
            return m_tunnelId;
        }

        const StaticByteArray<1024>& TunnelData::getData() const
        {
            return m_data;
        }

        ByteArray TunnelData::compile() const
        {
            ByteArray b;

            b.insert(b.end(), m_tunnelId >> 24);
            b.insert(b.end(), m_tunnelId >> 16);
            b.insert(b.end(), m_tunnelId >> 8);
            b.insert(b.end(), m_tunnelId);

            b.insert(b.end(), m_data.cbegin(), m_data.cend());

            return b;
        }

        TunnelData TunnelData::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            TunnelData td;

            if(std::distance(begin,end) < (4 + 1024))
                throw std::runtime_error("invalid tunnel data message");

            td.m_tunnelId = parseUint32(begin);

            std::copy(begin, begin + 1024, td.m_data.begin());

            return td;
        }
    }
}
