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

			m_byteTotal += data.size();

			return true;
		}

		bool InboundMessageState::isComplete() const
		{
			if(!m_gotLast) return false;

			for(unsigned char f = 0; f < m_lastFragment; f++)
				if(m_fragments.count(f) < 1) return false;

			return true;
		}

		ByteArray InboundMessageState::assemble() const
		{
			ByteArray dst(m_byteTotal);

			if(!isComplete()) return dst;

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
