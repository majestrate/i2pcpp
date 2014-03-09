/**
 * @file BuildRequestRecord.cpp
 * @brief Implements BuildRequestRecord.h.
 */
#include <i2pcpp/datatypes/BuildRequestRecord.h>

#include <i2pcpp/datatypes/RouterIdentity.h>

#include <botan/auto_rng.h>

namespace i2pcpp {
    BuildRequestRecord::BuildRequestRecord(BuildRecord const &r) :
        BuildRecord(r) {}

    BuildRequestRecord::BuildRequestRecord(RouterIdentity const &local, RouterHash const &nextHash) :
        m_localHash(local.getHash()),
        m_nextHash(nextHash),
        m_encryptionKey(local.getEncryptionKey()),
        m_requestTime(std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count())
    {
        Botan::AutoSeeded_RNG rng;

        rng.randomize((unsigned char *)&m_nextTunnelId, sizeof(m_nextTunnelId));

        randomize();
    }

    BuildRequestRecord::BuildRequestRecord(RouterIdentity const &local, RouterHash const &nextHash, uint32_t const nextTunnelId) :
        m_localHash(local.getHash()),
        m_nextHash(nextHash),
        m_nextTunnelId(nextTunnelId),
        m_encryptionKey(local.getEncryptionKey()),
        m_requestTime(std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count())
    {
        randomize();
    }

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

    void BuildRequestRecord::setTunnelId(uint32_t tunnelId)
    {
        m_tunnelId = tunnelId;
    }

    void BuildRequestRecord::setNextTunnelId(uint32_t nextTunnelId)
    {
        m_nextTunnelId = nextTunnelId;
    }

    void BuildRequestRecord::setLocalHash(RouterHash const &localHash)
    {
        m_localHash = localHash;
    }

    void BuildRequestRecord::setNextHash(RouterHash const &nextHash)
    {
        m_nextHash = nextHash;
    }

    void BuildRequestRecord::setTunnelLayerKey(SessionKey const &tunnelLayerKey)
    {
        m_tunnelLayerKey = tunnelLayerKey;
    }

    void BuildRequestRecord::setTunnelIVKey(SessionKey const &tunnelIVKey)
    {
        m_tunnelIVKey = tunnelIVKey;
    }

    void BuildRequestRecord::setReplyKey(SessionKey const &replyKey)
    {
        m_replyKey = replyKey;
    }

    void BuildRequestRecord::setReplyIV(StaticByteArray<16> const &replyIV)
    {
        m_replyIV = replyIV;
    }

    void BuildRequestRecord::setType(BuildRequestRecord::Type type)
    {
        switch(type) {
            case BuildRequestRecord::Type::GATEWAY:
                m_flags[7] = true;
                break;

            case BuildRequestRecord::Type::ENDPOINT:
                m_flags[6] = true;
                break;

            case BuildRequestRecord::Type::PARTICIPANT:
                m_flags = 0;
                break;
        }
    }

    void BuildRequestRecord::setRequestTime(uint32_t reqTime)
    {
        m_requestTime = reqTime;
    }

    void BuildRequestRecord::setNextMsgId(uint32_t nextMsgId)
    {
        m_nextMsgId = nextMsgId;
    }

    uint32_t BuildRequestRecord::getTunnelId() const
    {
        return m_tunnelId;
    }

    uint32_t BuildRequestRecord::getNextTunnelId() const
    {
        return m_nextTunnelId;
    }

    RouterHash BuildRequestRecord::getLocalHash() const
    {
        return m_localHash;
    }

    RouterHash BuildRequestRecord::getNextHash() const
    {
        return m_nextHash;
    }

    SessionKey BuildRequestRecord::getTunnelLayerKey() const
    {
        return m_tunnelLayerKey;
    }

    SessionKey BuildRequestRecord::getTunnelIVKey() const
    {
        return m_tunnelIVKey;
    }

    SessionKey BuildRequestRecord::getReplyKey() const
    {
        return m_replyKey;
    }

    StaticByteArray<16> BuildRequestRecord::getReplyIV() const
    {
        return m_replyIV;
    }

    BuildRequestRecord::Type BuildRequestRecord::getType() const
    {
        if(m_flags[7])
            return BuildRequestRecord::Type::GATEWAY;
        else if(m_flags[6])
            return BuildRequestRecord::Type::ENDPOINT;
        else
            return BuildRequestRecord::Type::PARTICIPANT;
    }

    uint32_t BuildRequestRecord::getRequestTime() const
    {
        return m_requestTime;
    }

    uint32_t BuildRequestRecord::getNextMsgId() const
    {
        return m_nextMsgId;
    }

    ByteArray BuildRequestRecord::getEncryptionKey() const
    {
        return m_encryptionKey;
    }

    void BuildRequestRecord::randomize()
    {
        Botan::AutoSeeded_RNG rng;

        rng.randomize((unsigned char *)&m_tunnelId, sizeof(m_tunnelId));
        rng.randomize(m_tunnelLayerKey.data(), m_tunnelLayerKey.size());
        rng.randomize(m_tunnelIVKey.data(), m_tunnelIVKey.size());
        rng.randomize(m_replyKey.data(), m_replyKey.size());
        rng.randomize(m_replyIV.data(), m_replyIV.size());
        rng.randomize((unsigned char *)&m_nextMsgId, sizeof(m_nextMsgId));
    }
}
