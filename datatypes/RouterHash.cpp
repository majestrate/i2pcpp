#include "RouterHash.h"

#include "../util/Base64.h"

namespace i2pcpp {
	RouterHash::RouterHash() :
		std::array<unsigned char, 32>() {}

	RouterHash::RouterHash(ByteArray const &b)
	{
		std::copy(b.cbegin(), b.cbegin() + 32, begin());
	}

	RouterHash::operator ByteArray() const
	{
		return ByteArray(cbegin(), cend());
	}

	std::ostream& operator<<(std::ostream &s, RouterHash const &rh)
	{
		s << Base64::encode(rh);
		return s;
	}
}
