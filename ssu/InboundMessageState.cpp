#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		void InboundMessageState::addFragment(const unsigned char fragNum, ByteArray const &data)
		{
			m_fragments[fragNum] = data;
		}
	}
}
