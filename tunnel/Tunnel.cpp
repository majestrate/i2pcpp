#include "Tunnel.h"

namespace i2pcpp {
	Tunnel::Tunnel(Direction d, std::vector<RouterIdentity> const &hops, RouterHash const &myHash) :
		m_direction(d)
	{
		if(d == OUTBOUND) {
			for(int i = 0; i < hops.size(); i++) {
				if(i == hops.size() - 1) {
					auto h = std::make_shared<TunnelHop>(hops[i], myHash);
					h->setType(TunnelHop::ENDPOINT);
					m_hops.push_back(h);
				} else
					m_hops.push_back(std::make_shared<TunnelHop>(hops[i], hops[i + 1].getHash()));
			}
		}
	}

	Tunnel::State Tunnel::getState() const
	{
		return m_state;
	}

	std::list<BuildRecordPtr> Tunnel::getRecords() const
	{
		std::list<BuildRecordPtr> recs;

		for(auto h = m_hops.cbegin(); h != m_hops.cend(); ++h) {
			auto record = std::make_shared<BuildRequestRecord>(*h);

			const RouterHash hopHash = (*h)->getLocalHash();
			std::array<unsigned char, 16> truncatedHash;
			std::copy(hopHash.cbegin(), hopHash.cbegin() + 16, truncatedHash.begin());
			record->setHeader(truncatedHash);

			record->compile();
			record->encrypt((*h)->getEncryptionKey());

			std::list<TunnelHopPtr>::const_reverse_iterator r(h);
			for(; r != m_hops.crend(); ++r)
				record->decrypt((*r)->getReplyIV(), (*r)->getReplyKey());

			recs.emplace_back(record);
		}

		return recs;
	}

	RouterHash Tunnel::getDownstream() const
	{
		return m_hops.front()->getLocalHash();
	}
}
