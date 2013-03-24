#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		InboundMessageState::InboundMessageState(RouterHash const &rh, const uint32_t msgId) :
			m_routerHash(rh),
			m_msgId(msgId) {}

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

			if(m_states.size() < fragNum + 1)
				m_states.resize(fragNum + 1);
			m_states.markA(fragNum);

			m_byteTotal += data.size();
		}

		ByteArray InboundMessageState::assemble() const
		{
			ByteArray dst(m_byteTotal);

			auto itr = dst.begin();
			for(auto fp: m_fragments)
			{
				ByteArray f = fp.second;
				copy(f.cbegin(), f.cend(), itr);
				itr += f.size();
			}

			return dst;
		}


		RouterHash InboundMessageState::getRouterHash() const
		{
			return m_routerHash;
		}

		uint32_t InboundMessageState::getMsgId() const
		{
			return m_msgId;
		}

		bool InboundMessageState::allFragmentsReceived() const
		{
			if(!m_gotLast) return false;

			return m_states.allA();
		}

		std::vector<bool> InboundMessageState::getAckStates() const
		{
			return m_states.getB();
		}
	}
}
