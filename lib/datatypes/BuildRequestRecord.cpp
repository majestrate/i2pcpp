/**
 * @file BuildRequestRecord.cpp
 * @brief Implements BuildRequestRecord.h.
 */
#include <i2pcpp/datatypes/BuildRequestRecord.h>

namespace i2pcpp {
    BuildRequestRecord::BuildRequestRecord(BuildRecord const &r) :
        BuildRecord(r) {}

    TunnelHop BuildRequestRecord::parse()
    {
        auto dataItr = m_data.cbegin();

        TunnelHop hop;

        uint32_t id, requestTime, nextMsgId;
        RouterHash rh;
        SessionKey sk;
        StaticByteArray<16> iv;

        id = parseUint32(dataItr);
        hop.setTunnelId(id);

        std::copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
        hop.setLocalHash(rh);

        id = parseUint32(dataItr);
        hop.setNextTunnelId(id);

        std::copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
        hop.setNextHash(rh);

        std::copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
        hop.setTunnelLayerKey(sk);

        std::copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
        hop.setTunnelIVKey(sk);

        std::copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
        hop.setReplyKey(sk);

        std::copy(dataItr, dataItr + 16, iv.begin()), dataItr += 16;
        hop.setReplyIV(iv);

        m_flags = *(dataItr)++;
        if(m_flags[7])
            hop.setType(TunnelHop::Type::GATEWAY);
        else if(m_flags[6])
            hop.setType(TunnelHop::Type::ENDPOINT);
        else
            hop.setType(TunnelHop::Type::PARTICIPANT);

        requestTime = parseUint32(dataItr);
        hop.setRequestTime(requestTime);

        nextMsgId = parseUint32(dataItr);
        hop.setNextMsgId(nextMsgId);

        return std::move(hop);
    }

    void BuildRequestRecord::compile(TunnelHop const &hop)
    {
        auto dataItr = m_data.begin();

        uint32_t tunnelId = hop.getTunnelId();
        *dataItr = tunnelId >> 24, dataItr++;
        *dataItr = tunnelId >> 16, dataItr++;
        *dataItr = tunnelId >> 8, dataItr++;
        *dataItr = tunnelId, dataItr++;

        RouterHash localHash = hop.getLocalHash();
        std::copy(localHash.cbegin(), localHash.cend(), dataItr);
        dataItr += 32;

        uint32_t nextTunnelId = hop.getNextTunnelId();
        *dataItr = nextTunnelId >> 24, dataItr++;
        *dataItr = nextTunnelId >> 16, dataItr++;
        *dataItr = nextTunnelId >> 8, dataItr++;
        *dataItr = nextTunnelId, dataItr++;

        RouterHash nextHash = hop.getNextHash();
        std::copy(nextHash.cbegin(), nextHash.cend(), dataItr);
        dataItr += 32;

        SessionKey tunnelLayerKey = hop.getTunnelLayerKey();
        std::copy(tunnelLayerKey.cbegin(), tunnelLayerKey.cend(), dataItr);
        dataItr += 32;

        SessionKey tunnelIVKey = hop.getTunnelIVKey();
        std::copy(tunnelIVKey.cbegin(), tunnelIVKey.cend(), dataItr);
        dataItr += 32;

        SessionKey replyKey = hop.getReplyKey();
        std::copy(replyKey.cbegin(), replyKey.cend(), dataItr);
        dataItr += 32;

        StaticByteArray<16> replyIV = hop.getReplyIV();
        std::copy(replyIV.cbegin(), replyIV.cend(), dataItr);
        dataItr += 16;

        switch(hop.getType()) {
            case TunnelHop::Type::GATEWAY:
                m_flags[7] = true;
                break;

            case TunnelHop::Type::ENDPOINT:
                m_flags[6] = true;
                break;

            case TunnelHop::Type::PARTICIPANT:
                m_flags = 0;
                break;
        }

        *dataItr = static_cast<unsigned char>(m_flags.to_ulong()), dataItr++;

        uint32_t requestTime = hop.getRequestTime();
        *dataItr = requestTime >> 24, dataItr++;
        *dataItr = requestTime >> 16, dataItr++;
        *dataItr = requestTime >> 8, dataItr++;
        *dataItr = requestTime, dataItr++;

        uint32_t nextMsgId = hop.getNextMsgId();
        *dataItr = nextMsgId >> 24, dataItr++;
        *dataItr = nextMsgId >> 16, dataItr++;
        *dataItr = nextMsgId >> 8, dataItr++;
        *dataItr = nextMsgId, dataItr++;

        std::fill(dataItr, m_data.end(), 0x00); // TODO Random padding
    }
}
