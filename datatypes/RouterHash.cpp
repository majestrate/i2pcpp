#include "RouterHash.h"

std::ostream& operator<<(std::ostream &s, i2pcpp::RouterHash const &rh)
{
	s << i2pcpp::Base64::encode(i2pcpp::ByteArray(rh.cbegin(), rh.cend()));
	return s;
}
