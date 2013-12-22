#include "TunnelData.h"

namespace i2pcpp {
    namespace I2NP {
        TunnelData::TunnelData(uint32_t const tunnelId, ByteArray const &data) :
            m_tunnelId(tunnelId)
        {
            std::copy(data.cbegin(), data.cbegin() + 1024, m_data.begin());
        }

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

            if(end - begin < (4 + 1024))
                throw std::runtime_error("invalid tunnel data message");

            td.m_tunnelId = (begin[0] << 24) | (begin[1] << 16) | (begin[2] << 8) | (begin[3]);
            begin += 4;

            std::copy(begin, begin + 1024, td.m_data.begin());

            return td;
        }
    }
}
