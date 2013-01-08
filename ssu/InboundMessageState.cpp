#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
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

		bool InboundMessageState::allFragmentsReceived() const
		{
			if(!m_gotLast) return false;

			return (m_fragmentAckStates.count() == m_fragmentAckStates.size());;
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
	}
}
