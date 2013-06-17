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
			RouterHash(std::string const &s);
			RouterHash(std::array<unsigned char, 32> const &k);

			operator ByteArray() const;
			operator std::string() const;
	};

	std::ostream& operator<<(std::ostream &s, RouterHash const &rh);
}

namespace std {
	template<>
	struct hash<i2pcpp::RouterHash> {
		public:
			size_t operator()(const i2pcpp::RouterHash &rh) const
			{
				hash<string> f;
				return f(string(rh.cbegin(), rh.cend()));
			}
	};
}

#endif
