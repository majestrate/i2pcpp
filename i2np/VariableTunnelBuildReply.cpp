#include "VariableTunnelBuildReply.h"

#include "../Log.h"

namespace i2pcpp {
	namespace I2NP {
		VariableTunnelBuildReply::VariableTunnelBuildReply(std::list<BuildRecordPtr> const &buildRecords) :
			m_buildRecords(buildRecords) {}

		VariableTunnelBuildReply::VariableTunnelBuildReply(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords) :
			Message(msgId),
			m_buildRecords(buildRecords) {}

		std::list<BuildRecordPtr> VariableTunnelBuildReply::getRecords() const
		{
			return m_buildRecords;
		}

		ByteArray VariableTunnelBuildReply::compile() const
		{
			ByteArray b;

			b.insert(b.end(), m_buildRecords.size());
			for(auto& r: m_buildRecords) {
				const ByteArray recordBytes = r->serialize();
				b.insert(b.end(), recordBytes.cbegin(), recordBytes.cend());
			}

			return b;
		}

		VariableTunnelBuildReply VariableTunnelBuildReply::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			VariableTunnelBuildReply vtbr;

			unsigned char size = *begin++;

			for(int i = 0; i < size; i++)
				vtbr.m_buildRecords.emplace_back(std::make_shared<BuildRecord>(begin, end));

			return vtbr;
		}
	}
}
