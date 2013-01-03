#ifndef SESSIONKEY_H
#define SESSIONKEY_H

#include <array>

#include "RouterHash.h"

namespace i2pcpp {
	class SessionKey : public std::array<unsigned char, 32> {
		public:
			SessionKey() : std::array<unsigned char, 32>() {}
			SessionKey(ByteArray const &b) { copy(b.cbegin(), b.cbegin() + 32, begin()); }

			SessionKey& operator=(RouterHash const &rh);
	};
}

#endif
