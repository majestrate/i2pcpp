#include "Certificate.h"

namespace i2pcpp {
	Certificate::Certificate(ByteArray::const_iterator &certItr)
	{
		m_type = (Type)*(certItr++);
	
		unsigned short size = (*(certItr++) << 8) | *(certItr++);
		copy(certItr, certItr + size, m_payload.begin()), certItr += size;
	}

	ByteArray Certificate::getBytes() const
	{
		ByteArray b(m_payload);
		unsigned short length = m_payload.size();

		b.insert(b.begin(), length);
		b.insert(b.begin(), (length >> 8));
		b.insert(b.begin(), (unsigned char)m_type);

		return b;
	}
}
