#include "Tunnel.h"

#include <i2pcpp/datatypes/BuildResponseRecord.h>

namespace i2pcpp {
    Tunnel::State Tunnel::getState() const
    {
        return m_state;
    }

    uint32_t Tunnel::getTunnelId() const
    {
        return m_tunnelId;
    }

    std::list<BuildRecordPtr> Tunnel::getRecords() const
    {
        return m_hops;
    }

    RouterHash Tunnel::getDownstream() const
    {
        return std::static_pointer_cast<BuildRequestRecord>(m_hops.front())->getLocalHash();
    }

    uint32_t Tunnel::getNextMsgId() const
    {
        return m_nextMsgId;
    }

    void Tunnel::handleResponses(std::list<BuildRecordPtr> const &records)
    {
        bool allgood = true;

        auto itr = m_hops.crbegin();
        ++itr;

        for(; itr != m_hops.crend(); ++itr) {
            BuildRequestRecordPtr h = std::static_pointer_cast<BuildRequestRecord>(*itr);

            for(auto r: records)
                r->decrypt(h->getReplyIV(), h->getReplyKey());

            for(auto r: records) {
                BuildResponseRecord resp = *r;
                resp.parse();
                if(resp.getReply() == BuildResponseRecord::Reply::SUCCESS) {
                    // TODO Record the success in the router's profile.
                } else {
                    // TODO Record the failure in the router's profile.
                    allgood = false;
                }
            }
        }

        if(allgood)
            m_state = Tunnel::State::OPERATIONAL;
        else
            m_state = Tunnel::State::FAILED;
    }

    void Tunnel::secureRecords()
    {
        for(auto itr = m_hops.cbegin(); itr != m_hops.cend(); ++itr) {
            BuildRequestRecordPtr h = std::static_pointer_cast<BuildRequestRecord>(*itr);

            const RouterHash hopHash = h->getLocalHash();
            StaticByteArray<16> truncatedHash;
            std::copy(hopHash.cbegin(), hopHash.cbegin() + 16, truncatedHash.begin());
            h->setHeader(truncatedHash);

            h->compile();
            h->encrypt(h->getEncryptionKey());

            std::list<BuildRecordPtr>::const_reverse_iterator ritr(itr);
            for(; ritr != m_hops.crend(); ++ritr) {
                BuildRequestRecordPtr r = std::static_pointer_cast<BuildRequestRecord>(*ritr);
                h->decrypt(r->getReplyIV(), r->getReplyKey());
            }
        }
    }
}
