#include "VariableTunnelBuild.h"

#include <iostream>

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
			unsigned char size = *dataItr++;

			for(int i = 0; i < size; i++)
				m_buildRecords.emplace_back(dataItr);

			return true;
		}
	}
}
