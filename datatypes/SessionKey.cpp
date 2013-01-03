#include "SessionKey.h"

namespace i2pcpp {
	SessionKey& SessionKey::operator=(RouterHash const &rh)
	{
		std::copy(rh.cbegin(), rh.cbegin() + 32, begin());
		return *this;
	}
}
