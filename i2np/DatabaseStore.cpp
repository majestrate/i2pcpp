#include "DatabaseStore.h"

namespace i2pcpp {
    namespace I2NP {
        DatabaseStore::DatabaseStore(StaticByteArray<32> const &key, DataType type, uint32_t replyToken, ByteArray const &data) :
            Message(),
            m_key(key),
            m_type(type),
            m_replyToken(replyToken),
            m_data(data) {}

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

        ByteArray DatabaseStore::compile() const
        {
            ByteArray b;

            b.insert(b.end(), m_key.cbegin(), m_key.cend());
            b.insert(b.end(), (unsigned char)m_type);

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

        DatabaseStore DatabaseStore::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            DatabaseStore ds;

            std::copy(begin, begin + 32, ds.m_key.begin());
            begin += 32;

            ds.m_type = (DataType)*(begin++);
            ds.m_replyToken = parseUint32(begin);

            if(ds.m_replyToken) {
                ds.m_replyTunnelId = parseUint32(begin);

                std::copy(begin, begin + 32, ds.m_replyGateway.begin());
                begin += 32;
            }

            uint16_t size = parseUint16(begin);

            ds.m_data = ByteArray(begin, begin + size);

            return ds;
        }
    }
}
