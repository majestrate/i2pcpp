#include "SessionTag.h"

namespace i2pcpp {
	SessionTag::SessionTag()
	{
	}

	ByteArray SessionTag::getBytes() const
	{
		ByteArray b(32);

		copy(m_tag.begin(), m_tag.end(), b.begin());

		return b;
	}
}
