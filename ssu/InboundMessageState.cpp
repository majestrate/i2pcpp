#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		bool InboundMessageState::addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast)
		{
			if(m_gotLast && fragNum > m_lastFragment)
				return false;

			if(m_fragments.count(fragNum) > 0)
				return true;

			m_fragments[fragNum] = data;

			if(isLast) {
				m_gotLast = true;
				m_lastFragment = fragNum;
			}

			return true;
		}

		bool InboundMessageState::isComplete() const
		{
			if(!m_gotLast) return false;

			for(unsigned char f; f < m_lastFragment; f++)
				if(m_fragments.count(f) < 1) return false;

			return true;
		}
	}
}
