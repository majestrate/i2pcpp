#include "Date.h"

namespace i2pcpp {
	Date::Date() : m_value(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {}

	Date::Date(ByteArray::const_iterator &dateItr) : m_value(0)
	{
		for(int i = sizeof(unsigned long long); i > 0; i--)
			m_value = (m_value << 8) + (*(dateItr++));
	}

	ByteArray Date::getBytes() const
	{
		ByteArray v;

		for(int i = sizeof(unsigned long long); i > 0; i--)
			v.push_back((m_value >> ((i - 1) * 8)));

		return v;
	}
}
