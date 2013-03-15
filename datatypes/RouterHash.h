#ifndef ROUTERHASH_H
#define ROUTERHASH_H

#include <array>
#include <string>
#include <functional>
#include <iostream>

#include "ByteArray.h"

namespace i2pcpp {
	class RouterHash : public std::array<unsigned char, 32> {
		public:
			RouterHash();
			RouterHash(ByteArray const &b);

			operator ByteArray() const;
	};
}

std::ostream& operator<<(std::ostream &s, i2pcpp::RouterHash const &rh);

template<>
struct std::hash<i2pcpp::RouterHash> {
	public:
		size_t operator()(const i2pcpp::RouterHash &rh) const
		{
			std::hash<std::string> f;
			return f(std::string(rh.cbegin(), rh.cend()));
		}
};

#endif
