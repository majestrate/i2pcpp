#include "Tunnel.h"

#include "../Log.h"

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
