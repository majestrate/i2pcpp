/**
 * @file DeliveryStatus.cpp
 * @brief Implements DeliveryStatus.h
 */
#include "DeliveryStatus.h"

namespace i2pcpp {
    namespace I2NP {
        DeliveryStatus::DeliveryStatus(uint32_t msgId, Date timestamp) :
            m_msgId(msgId),
            m_timestamp(timestamp) {}

        ByteArray DeliveryStatus::compile() const
        {
            ByteArray b;

            b.insert(b.end(), m_msgId >> 24);
            b.insert(b.end(), m_msgId >> 16);
            b.insert(b.end(), m_msgId >> 8);
            b.insert(b.end(), m_msgId);

            ByteArray ts = m_timestamp.serialize();
            b.insert(b.end(), ts.cbegin(), ts.cend());

            return b;
        }

        DeliveryStatus DeliveryStatus::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            DeliveryStatus ds;

            ds.m_msgId = parseUint32(begin);

            ds.m_timestamp = Date(begin, end);

            return ds;
        }
    }
}
