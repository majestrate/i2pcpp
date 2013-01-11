#include "VariableTunnelBuild.h"

namespace i2pcpp {
	namespace I2NP {
		ByteArray VariableTunnelBuild::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(), m_buildRecords.size());
			for(auto& r: m_buildRecords) {
				const ByteArray&& recordBytes = r.getBytes();
				b.insert(b.end(), recordBytes.cbegin(), recordBytes.cend());
			}

			return b;
		}

		bool VariableTunnelBuild::parse(ByteArray::const_iterator &dataItr)
		{
			return false;
		}
	}
}
