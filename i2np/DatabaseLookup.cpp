#include "DatabaseLookup.h"

namespace i2pcpp {
	namespace I2NP {
		DatabaseLookup::DatabaseLookup() {}

		DatabaseLookup::DatabaseLookup(std::array<unsigned char, 32> const &key, RouterHash const &from, uint32_t sendReplyTo, std::list<RouterHash> excludedPeers) :
			m_key(key),
			m_from(from),
			m_sendReplyTo(sendReplyTo),
			m_excludedPeers(excludedPeers) {}

		Message::Type DatabaseLookup::getType() const
		{
			return Message::Type::DB_LOOKUP;
		}

		ByteArray DatabaseLookup::getBytes() const
		{
			ByteArray b;

			b.insert(b.end(), m_key.cbegin(), m_key.cend());
			b.insert(b.end(), m_from.cbegin(), m_from.cend());

			if(m_sendReplyTo) {
				b.insert(b.end(), 0x01);

				b.insert(b.end(), m_sendReplyTo >> 24);
				b.insert(b.end(), m_sendReplyTo >> 16);
				b.insert(b.end(), m_sendReplyTo >> 8);
				b.insert(b.end(), m_sendReplyTo);
			} else
				b.insert(b.end(), 0x00);

			uint16_t size = m_excludedPeers.size();
			b.insert(b.end(), size >> 8);
			b.insert(b.end(), size);

			for(auto p: m_excludedPeers)
				b.insert(b.end(), p.cbegin(), p.cend());

			return b;
		}

		bool DatabaseLookup::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
		{
			return false;
		}
	}
}
