#include "RouterAddress.h"

namespace i2pcpp {
	RouterAddress::RouterAddress(ByteArray::const_iterator &addrItr)
	{
		m_cost = *(addrItr++);
		m_expiration = Date(addrItr);

		unsigned char size = *(addrItr++);
		m_transport = std::string(addrItr, addrItr + size);
		addrItr += size;

		m_options = Mapping(addrItr);
	}

	ByteArray RouterAddress::getBytes() const
	{
		ByteArray b;

		const ByteArray& expiration = m_expiration.getBytes();
		const ByteArray& options = m_options.getBytes();

		b.insert(b.end(), m_cost);
		b.insert(b.end(), expiration.cbegin(), expiration.cend());
		b.insert(b.end(), m_transport.size());
		b.insert(b.end(), m_transport.cbegin(), m_transport.cend());
		b.insert(b.end(), options.cbegin(), options.cend());

		return b;
	}
}
