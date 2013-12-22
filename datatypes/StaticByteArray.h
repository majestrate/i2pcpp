#ifndef STATICBYTEARRAY_H
#define STATICBYTEARRAY_H

#include <algorithm>
#include <array>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "../util/Base64.h"

#include "ByteArray.h"

namespace i2pcpp {
	template<std::size_t L>
		class StaticByteArray : public std::array<unsigned char, L> {
			public:
				StaticByteArray() = default;

				~StaticByteArray()
				{
					for(auto& x: *this)
						x = 0;
				}

				StaticByteArray(ByteArray const &b)
				{
					if(b.size() < L)
						std::copy(b.cbegin(), b.cend(), this->begin());
					else
						std::copy(b.cbegin(), b.cbegin() + L, this->begin());
				}

				StaticByteArray(std::string const &s)
				{
					ByteArray b = Base64::decode(s);
					if(b.size() != L) throw std::runtime_error("input string not correct size for StaticByteArray");
					std::copy(b.cbegin(), b.cbegin() + L, this->begin());
				}

				std::string toHex() const
				{
					std::stringstream s;
					for(auto c: *this)
						s << std::setw(2) << std::setfill('0') << std::hex << (int)c;

					return s.str();
				}

				operator ByteArray() const
				{
					return ByteArray(this->cbegin(), this->cend());
				}

				operator std::string() const
				{
					std::string b64;
					b64 = Base64::encode(ByteArray(this->cbegin(), this->cend()));
					std::replace(b64.begin(), b64.end(), '+', '-');
					std::replace(b64.begin(), b64.end(), '/', '~');

					return b64;
				}
		};

	template<std::size_t L>
	std::ostream& operator<<(std::ostream &s, StaticByteArray<L> const &sba)
	{
		s << std::string(sba);
		return s;
	}
}

namespace std {
	template<std::size_t L>
		struct hash<i2pcpp::StaticByteArray<L>> {
			public:
				size_t operator()(const i2pcpp::StaticByteArray<L> &sba) const
				{
					hash<string> f;
					return f(sba);
				}
		};
}

#endif
