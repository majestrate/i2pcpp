#include "DatabaseSearchReply.h"

namespace i2pcpp {
	namespace I2NP {
		const StaticByteArray<32>& DatabaseSearchReply::getKey() const
		{
			return m_key;
		}

		const std::list<RouterHash>& DatabaseSearchReply::getHashes() const
		{
			return m_hashes;
		}

		const RouterHash& DatabaseSearchReply::getFrom() const
		{
			return m_from;
		}

		ByteArray DatabaseSearchReply::compile() const
		{
			// TODO
			return ByteArray();
		}

		DatabaseSearchReply DatabaseSearchReply::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			DatabaseSearchReply dsr;

			std::copy(begin, begin + 32, dsr.m_key.begin()), begin += 32;

			unsigned char size = *(begin++);
			while(size--) {
				dsr.m_hashes.emplace_back();
				std::copy(begin, begin + 32, dsr.m_hashes.back().begin()), begin += 32;
			}

			std::copy(begin, begin + 32, dsr.m_from.begin()), begin += 32;

			return dsr;
		}
	}
}
