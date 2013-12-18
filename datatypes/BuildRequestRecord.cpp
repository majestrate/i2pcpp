#include "BuildRequestRecord.h"

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	BuildRequestRecord::BuildRequestRecord(BuildRecord const &r) :
		BuildRecord(r) {}

	TunnelHop BuildRequestRecord::parse()
	{
		uint32_t id, requestTime, nextMsgId;
		RouterHash rh;
		SessionKey sk;
		StaticByteArray<16> iv;
		auto dataItr = m_data.cbegin();

		if(m_data.size() != 222)
			throw FormattingError();

		TunnelHop hop;
		id = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		hop.setTunnelId(id);

		copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
		hop.setLocalHash(rh);

		id = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		hop.setNextTunnelId(id);

		copy(dataItr, dataItr + 32, rh.begin()), dataItr += 32;
		hop.setNextHash(rh);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		hop.setTunnelLayerKey(sk);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		hop.setTunnelIVKey(sk);

		copy(dataItr, dataItr + 32, sk.begin()), dataItr += 32;
		hop.setReplyKey(sk);

		copy(dataItr, dataItr + 16, iv.begin()), dataItr += 16;
		hop.setReplyIV(iv);

		m_flags = *(dataItr)++;
		if(m_flags[7])
			hop.setType(TunnelHop::Type::GATEWAY);
		else if(m_flags[6])
			hop.setType(TunnelHop::Type::ENDPOINT);
		else
			hop.setType(TunnelHop::Type::PARTICIPANT);

		requestTime = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		hop.setRequestTime(requestTime);

		nextMsgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		hop.setNextMsgId(nextMsgId);

		return hop;
	}

	void BuildRequestRecord::compile(TunnelHop const &hop)
	{
		m_data.clear();

		uint32_t tunnelId = hop.getTunnelId();
		m_data.insert(m_data.end(), tunnelId >> 24);
		m_data.insert(m_data.end(), tunnelId >> 16);
		m_data.insert(m_data.end(), tunnelId >> 8);
		m_data.insert(m_data.end(), tunnelId);

		RouterHash localHash = hop.getLocalHash();
		m_data.insert(m_data.end(), localHash.cbegin(), localHash.cend());

		uint32_t nextTunnelId = hop.getNextTunnelId();
		m_data.insert(m_data.end(), nextTunnelId >> 24);
		m_data.insert(m_data.end(), nextTunnelId >> 16);
		m_data.insert(m_data.end(), nextTunnelId >> 8);
		m_data.insert(m_data.end(), nextTunnelId);

		RouterHash nextHash = hop.getNextHash();
		m_data.insert(m_data.end(), nextHash.cbegin(), nextHash.cend());

		SessionKey tunnelLayerKey = hop.getTunnelLayerKey();
		m_data.insert(m_data.end(), tunnelLayerKey.cbegin(), tunnelLayerKey.cend());

		SessionKey tunnelIVKey = hop.getTunnelIVKey();
		m_data.insert(m_data.end(), tunnelIVKey.cbegin(), tunnelIVKey.cend());

		SessionKey replyKey = hop.getReplyKey();
		m_data.insert(m_data.end(), replyKey.cbegin(), replyKey.cend());

		StaticByteArray<16> replyIV = hop.getReplyIV();
		m_data.insert(m_data.end(), replyIV.cbegin(), replyIV.cbegin() + 16);

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

		m_data.insert(m_data.end(), static_cast<unsigned char>(m_flags.to_ulong()));

		uint32_t requestTime = hop.getRequestTime();
		m_data.insert(m_data.end(), requestTime >> 24);
		m_data.insert(m_data.end(), requestTime >> 16);
		m_data.insert(m_data.end(), requestTime >> 8);
		m_data.insert(m_data.end(), requestTime);

		uint32_t nextMsgId = hop.getNextMsgId();
		m_data.insert(m_data.end(), nextMsgId >> 24);
		m_data.insert(m_data.end(), nextMsgId >> 16);
		m_data.insert(m_data.end(), nextMsgId >> 8);
		m_data.insert(m_data.end(), nextMsgId);

		m_data.insert(m_data.end(), 29, 0x00); // TODO Random padding
	}
}
