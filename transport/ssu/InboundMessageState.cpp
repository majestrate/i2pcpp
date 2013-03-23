#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageState::InboundMessageState(RouterHash const &routerHash, const uint32_t msgId) :
			m_msgId(msgId),
			m_routerHash(routerHash),
			m_gotLast(false),
			m_byteTotal(0) {}

		void InboundMessageState::addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast)
		{
			if(m_gotLast && fragNum > m_lastFragment)
				return; // TODO Exception

			if(m_fragments.count(fragNum) > 0)
				return; // TODO Exception

			m_fragments[fragNum] = data;

			if(isLast) {
				m_gotLast = true;
				m_lastFragment = fragNum;
			}

			if(m_fragmentAckStates.size() < fragNum + 1)
				m_fragmentAckStates.resize(fragNum + 1);
			m_fragmentAckStates.set(fragNum);

			m_byteTotal += data.size();
		}

		ByteArray InboundMessageState::assemble() const
		{
			ByteArray dst(m_byteTotal);

			if(!this) return dst; // TODO Exception

			auto itr = dst.begin();
			for(auto fp: m_fragments)
			{
				ByteArray f = fp.second;
				copy(f.cbegin(), f.cend(), itr);
				itr += f.size();
			}

			return dst;
		}

		bool InboundMessageState::allFragmentsReceived() const
		{
			if(!m_gotLast) return false;

			return (m_fragmentAckStates.count() == m_fragmentAckStates.size());;
		}

		uint32_t InboundMessageState::getMsgId() const
		{
			return m_msgId;
		}

		const RouterHash& InboundMessageState::getRouterHash() const
		{
			return m_routerHash;
		}

		unsigned char InboundMessageState::getNumFragments() const
		{
			return m_fragments.size();
		}

		const AckBitfield& InboundMessageState::getAckStates() const
		{
			return m_fragmentAckStates;
		}
	}
}
