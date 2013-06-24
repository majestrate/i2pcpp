#include "VariableTunnelBuild.h"

#include "../Log.h"

namespace i2pcpp {
	namespace I2NP {
		VariableTunnelBuild::VariableTunnelBuild() {}

		VariableTunnelBuild::VariableTunnelBuild(std::list<BuildRecord> const &buildRecords) :
			m_buildRecords(buildRecords) {}

		Message::Type VariableTunnelBuild::getType() const
		{
			return Message::Type::VARIABLE_TUNNEL_BUILD;
		}

		const std::list<BuildRecord>& VariableTunnelBuild::getRecords() const
		{
			return m_buildRecords;
		}

		ByteArray VariableTunnelBuild::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(),m_buildRecords.size());
			for(auto& r: m_buildRecords) {
				const ByteArray recordBytes = r.serialize();
				b.insert(b.end(), recordBytes.begin(), recordBytes.end());
			}

			return b;
		}

		bool VariableTunnelBuild::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			unsigned char size = *begin++;

			for(int i = 0; i < size; i++)
				m_buildRecords.emplace_back(begin, end);

			return true;
		}
	}
}
