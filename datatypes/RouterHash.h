#ifndef ROUTERHASH_H
#define ROUTERHASH_H

#include <array>
#include <string>
#include <functional>

#include "ByteArray.h"

namespace i2pcpp {
	class RouterHash : public std::array<unsigned char, 32> {
		public:
			RouterHash() : std::array<unsigned char, 32>() {}
			RouterHash(ByteArray const &b) { std::copy(b.cbegin(), b.cbegin() + 32, begin()); }
	};
}

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
