#include "SessionKey.h"

namespace i2pcpp {
	SessionKey& SessionKey::operator=(RouterHash const &rh)
	{
		std::copy(rh.cbegin(), rh.cbegin() + 32, begin());
		return *this;
	}
}

std::ostream& operator<<(std::ostream &s, i2pcpp::SessionKey const &sk)
{
	s << i2pcpp::Base64::encode(i2pcpp::ByteArray(sk.cbegin(), sk.cend()));
	return s;
}
