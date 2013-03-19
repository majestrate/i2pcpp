#include "SessionKey.h"

#include "../util/Base64.h"

namespace i2pcpp {
	SessionKey::SessionKey() :
		std::array<unsigned char, 32>() {}

	SessionKey::SessionKey(std::array<unsigned char, 32> const &a) :
		std::array<unsigned char, 32>(a) {}

	SessionKey::SessionKey(ByteArray const &b)
	{
		if(b.size() < 32) throw std::logic_error("Invalid argument");
		copy(b.cbegin(), b.cbegin() + 32, begin());
	}

	// TODO Move constructor here too?
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
