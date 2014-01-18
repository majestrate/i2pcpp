/**
 * @file BuildRequestRecord.cpp
 * @brief Implements BuildRequestRecord.h.
 */
#include <i2pcpp/datatypes/BuildRequestRecord.h>

namespace i2pcpp {
    BuildRequestRecord::BuildRequestRecord(BuildRecord const &r) :
        BuildRecord(r) {}

    void BuildRequestRecord::parse()
    {
        auto dataItr = m_data.cbegin();

        m_tunnelId = parseUint32(dataItr);

        std::copy(dataItr, dataItr + 32, m_localHash.begin()), dataItr += 32;

        m_nextTunnelId = parseUint32(dataItr);

        std::copy(dataItr, dataItr + 32, m_nextHash.begin()), dataItr += 32;

        std::copy(dataItr, dataItr + 32, m_tunnelLayerKey.begin()), dataItr += 32;

        std::copy(dataItr, dataItr + 32, m_tunnelIVKey.begin()), dataItr += 32;

        std::copy(dataItr, dataItr + 32, m_replyKey.begin()), dataItr += 32;

        std::copy(dataItr, dataItr + 16, m_replyIV.begin()), dataItr += 16;

        m_flags = *(dataItr)++;

        m_requestTime = parseUint32(dataItr);

        m_nextMsgId = parseUint32(dataItr);
    }

    void BuildRequestRecord::compile()
    {
        auto dataItr = m_data.begin();

        *dataItr = m_tunnelId >> 24, dataItr++;
        *dataItr = m_tunnelId >> 16, dataItr++;
        *dataItr = m_tunnelId >> 8, dataItr++;
        *dataItr = m_tunnelId, dataItr++;

        std::copy(m_localHash.cbegin(), m_localHash.cend(), dataItr);
        dataItr += 32;

        *dataItr = m_nextTunnelId >> 24, dataItr++;
        *dataItr = m_nextTunnelId >> 16, dataItr++;
        *dataItr = m_nextTunnelId >> 8, dataItr++;
        *dataItr = m_nextTunnelId, dataItr++;

        std::copy(m_nextHash.cbegin(), m_nextHash.cend(), dataItr);
        dataItr += 32;

        std::copy(m_tunnelLayerKey.cbegin(), m_tunnelLayerKey.cend(), dataItr);
        dataItr += 32;

        std::copy(m_tunnelIVKey.cbegin(), m_tunnelIVKey.cend(), dataItr);
        dataItr += 32;

        std::copy(m_replyKey.cbegin(), m_replyKey.cend(), dataItr);
        dataItr += 32;

        std::copy(m_replyIV.cbegin(), m_replyIV.cend(), dataItr);
        dataItr += 16;

        *dataItr = static_cast<unsigned char>(m_flags.to_ulong()), dataItr++;

        *dataItr = m_requestTime >> 24, dataItr++;
        *dataItr = m_requestTime >> 16, dataItr++;
        *dataItr = m_requestTime >> 8, dataItr++;
        *dataItr = m_requestTime, dataItr++;

        *dataItr = m_nextMsgId >> 24, dataItr++;
        *dataItr = m_nextMsgId >> 16, dataItr++;
        *dataItr = m_nextMsgId >> 8, dataItr++;
        *dataItr = m_nextMsgId, dataItr++;

        std::fill(dataItr, m_data.end(), 0x00); // TODO Random padding
    }
}
