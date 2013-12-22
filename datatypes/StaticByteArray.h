/**
 * @file StaticByteArray.h
 * @brief Defines the i2pcpp::StaticByteArray type, which is an utility type.
 */
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
	/**
	 * Provides a std::array with additional utility functions.
	 */
	template<std::size_t L>
	struct StaticByteArray : public std::array<unsigned char, L> {
		StaticByteArray() = default;

		/**
		 * Clears the underlying data.
		 */
		~StaticByteArray()
		{
			this->fill(0);
		}

		/**
		 * Constructs from an i2pcpp::ByteArray.
		 */
		StaticByteArray(ByteArray const &b)
		{
			if(b.size() < L)
				std::copy(b.cbegin(), b.cend(), this->begin());
			else
				std::copy(b.cbegin(), b.cbegin() + L, this->begin());
		}

		/**
		 * Constructs from a base64-encoded std::string.
		 * @throw std::runtime_error if the input string is not of the correct size
		 */
		StaticByteArray(std::string const &s)
		{
			ByteArray b = Base64::decode(s);
			if(b.size() != L)
				throw std::runtime_error("input string not correct size for StaticByteArray");
			std::copy(b.cbegin(), b.cbegin() + L, this->begin());
		}

		/**
		 * @return a hexadecimal string representing the underlying data
		 */
		std::string toHex() const
		{
			std::stringstream s;
			for(const auto& c: *this)
				s << std::setw(2) << std::setfill('0') << std::hex << (int)c;

			return s.str();
		}

		/**
		 * Conversion to an i2pcpp::ByteArray.
		 */
		operator ByteArray() const
		{
			return ByteArray(this->cbegin(), this->cend());
		}

		/**
		 * Converts to a std::string, by using base64 encoding.
		 */
		operator std::string() const
		{
			std::string b64 = Base64::encode(ByteArray(this->begin(), this->cend()));
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
	template<size_t L>
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
