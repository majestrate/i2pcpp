#ifndef SESSIONKEY_H
#define SESSIONKEY_H

#include <array>

#include "RouterHash.h"

namespace i2pcpp {
	class SessionKey : public std::array<unsigned char, 32> {
		public:
			SessionKey();
			SessionKey(std::array<unsigned char, 32> const &a);
			SessionKey(ByteArray const &b);

			SessionKey& operator=(RouterHash const &rh);
	};
}

std::ostream& operator<<(std::ostream &s, i2pcpp::SessionKey const &sk);

#endif
