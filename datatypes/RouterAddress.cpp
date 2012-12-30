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

		ByteArray expiration = m_expiration.getBytes();
		ByteArray options = m_options.getBytes();

		b.insert(b.end(), m_cost);
		b.insert(b.end(), expiration.begin(), expiration.end());
		b.insert(b.end(), m_transport.size());
		b.insert(b.end(), m_transport.begin(), m_transport.end());
		b.insert(b.end(), options.begin(), options.end());

		return b;
	}
}
