#include "BuildRequestRecord.h"

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	BuildRequestRecord::BuildRequestRecord(TunnelHopPtr const &hop) :
		m_hop(*hop)
	{
		switch(m_hop.getType()) {
			case TunnelHop::GATEWAY:
				m_flags[7] = true;
				break;

			case TunnelHop::ENDPOINT:
				m_flags[6] = true;
				break;

			case TunnelHop::PARTICIPANT:
				m_flags = 0;
				break;
		}
	}

	BuildRequestRecord::BuildRequestRecord(BuildRecord const &r) :
		BuildRecord(r) {}

	void BuildRequestRecord::parse()
	{
		uint32_t id;
		RouterHash rh;
		SessionKey sk;
		auto dataItr = m_data.cbegin();

		if(m_data.size() != 222)
			throw FormattingError();

		id = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		m_hop.setTunnelId(id);

		copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
		m_hop.setLocalHash(rh);

		id = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		m_hop.setNextTunnelId(id);

		copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
		m_hop.setNextHash(rh);

 		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		m_hop.setTunnelLayerKey(sk);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		m_hop.setTunnelIVKey(sk);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		m_hop.setReplyKey(sk);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		m_hop.setReplyIV(sk);

		m_flags = *(dataItr)++;
		if(m_flags[7])
			m_hop.setType(TunnelHop::GATEWAY);
		else if(m_flags[6])
			m_hop.setType(TunnelHop::ENDPOINT);
		else
			m_hop.setType(TunnelHop::PARTICIPANT);

		m_requestTime = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		m_nextMsgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
	}

	void BuildRequestRecord::compile()
	{
		uint32_t tunnelId = m_hop.getTunnelId();
		m_data.insert(m_data.end(), tunnelId >> 24);
		m_data.insert(m_data.end(), tunnelId >> 16);
		m_data.insert(m_data.end(), tunnelId >> 8);
		m_data.insert(m_data.end(), tunnelId);

		RouterHash localHash = m_hop.getLocalHash();
		m_data.insert(m_data.end(), localHash.cbegin(), localHash.cend());

		uint32_t nextTunnelId = m_hop.getNextTunnelId();
		m_data.insert(m_data.end(), nextTunnelId >> 24);
		m_data.insert(m_data.end(), nextTunnelId >> 16);
		m_data.insert(m_data.end(), nextTunnelId >> 8);
		m_data.insert(m_data.end(), nextTunnelId);

		RouterHash nextHash = m_hop.getNextHash();
		m_data.insert(m_data.end(), nextHash.cbegin(), nextHash.cend());

		SessionKey tunnelLayerKey = m_hop.getTunnelLayerKey();
		m_data.insert(m_data.end(), tunnelLayerKey.cbegin(), tunnelLayerKey.cend());

		SessionKey tunnelIVKey = m_hop.getTunnelIVKey();
		m_data.insert(m_data.end(), tunnelIVKey.cbegin(), tunnelIVKey.cend());

		SessionKey replyKey = m_hop.getReplyKey();
		m_data.insert(m_data.end(), replyKey.cbegin(), replyKey.cend());

		SessionKey replyIV = m_hop.getReplyIV();
		m_data.insert(m_data.end(), replyIV.cbegin(), replyIV.cbegin() + 16);

		m_data.insert(m_data.end(), static_cast<unsigned char>(m_flags.to_ulong()));

		m_data.insert(m_data.end(), m_requestTime >> 24);
		m_data.insert(m_data.end(), m_requestTime >> 16);
		m_data.insert(m_data.end(), m_requestTime >> 8);
		m_data.insert(m_data.end(), m_requestTime);

		m_data.insert(m_data.end(), m_nextMsgId >> 24);
		m_data.insert(m_data.end(), m_nextMsgId >> 16);
		m_data.insert(m_data.end(), m_nextMsgId >> 8);
		m_data.insert(m_data.end(), m_nextMsgId);

		m_data.insert(m_data.end(), 29, 0x00); // TODO Random padding
	}

	TunnelHop& BuildRequestRecord::getHop()
	{
		return m_hop;
	}
}
