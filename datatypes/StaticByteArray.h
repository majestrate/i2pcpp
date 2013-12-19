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
    template<unsigned int L>
	class StaticByteArray {
		public:
            typedef unsigned char value_type;
            typedef typename std::array<value_type, L>::size_type size_type;

			StaticByteArray()
            {
                m_data.fill(0);
            }

			StaticByteArray(const StaticByteArray &) = default;
			StaticByteArray& operator=(const StaticByteArray &) = default;
			StaticByteArray(StaticByteArray &&) = default;
			StaticByteArray& operator=(StaticByteArray &&) = default;

            /**
             * Clears the underlying data.
             */
			~StaticByteArray()
			{
				m_data.fill(0);
			}

            /**
             * Constrcuts from an i2pcpp::ByteArray.
             */
			StaticByteArray(ByteArray const &b)
			{
				if(b.size() < L)
					std::copy(b.cbegin(), b.cbegin() + b.size(), m_data.begin());
				else
					std::copy(b.cbegin(), b.cbegin() + L, m_data.begin());
			}

            /**
             * Constructs from a base64-encoded std::string.
             * @throw std::runtime_error
             */
			StaticByteArray(std::string const &s)
			{		
                ByteArray b = Base64::decode(s);
				ff(b.size() != L)
                    throw std::runtime_error("input string not correct size for StaticByteArray");
				std::copy(b.cbegin(), b.cbegin() + L, m_data.begin());
			}

            /**
             * Constructs from a std::array.
             */
			StaticByteArray(std::array<unsigned char, L> const &a)
			{
				std::copy(a.cbegin(), a.cbegin() + L, m_data.begin());
			}

            /**
             * @return a hexadecimal string representing the underlying data
             */
            std::string toHex() const
		    {
		    	std::stringstream s;
		    	for(const auto& c: m_data)
                    s << std::setw(2) << std::setfill('0') << std::hex << (int)c;

		    	return s.str();
		    }

            /**
             * @return an iterator to the begin of the underlying std::array object.
             */
			typename std::array<unsigned char, L>::iterator begin()
			{
				return m_data.begin();
			}

            /**
             * @return an iterator to the end of the underlying std::array object.
             */
			typename std::array<unsigned char, L>::iterator end()
			{
				return m_data.end();
			}

            /**
             * @return a constant iterator to the begin of the underlying std::array object.
             */
			typename std::array<unsigned char, L>::const_iterator cbegin() const
			{
				return m_data.cbegin();
			}

            /**
             * @return a constant iterator to the end of the underlying std::array object.
             */
			typename std::array<unsigned char, L>::const_iterator cend() const
			{
				return m_data.cend();
			}

            /**
             * @return a pointer to the data of the underlying std::array
             */
			typename std::array<unsigned char, L>::pointer data()
			{
				return m_data.data();
			}

            /**
             * @return a constant pointer to the data of the underlying std::array
             */
			typename std::array<unsigned char, L>::const_pointer data() const
			{
				return m_data.data();
			}

            /**
             * @return the size of the underlying std::array
             */
			typename std::array<unsigned char, L>::size_type size() const
			{
				return m_data.size();
			}

            /**
             * Compares to i2pcpp::StaticByteArray objects lexicographically.
             */
			bool operator<(const StaticByteArray<L> &sba) const
			{
				return m_data < sba.m_data;
			}

            /**
             * Subscripting operator for element access.
             */
			typename std::array<unsigned char, L>::const_reference operator[](typename std::array<unsigned char, L>::size_type n) const
			{
				return m_data[n];
			}

            /**
             * @see i2pcpp::StaticByteArray::operator==
             */
			bool operator!=(const StaticByteArray<L> &sba) const
			{
				return m_data != sba.m_data;
			}

            /**
             * Defines equality of two i2pcpp::StaticByteArray objects as
             *  containing the same data.
             */
			bool operator==(const StaticByteArray<L> &sba) const
			{
				return m_data == sba.m_data;
			}

            /**
             * Conversion to an i2pcpp::ByteArray.
             */
			operator ByteArray() const
			{
				return ByteArray(m_data.cbegin(), m_data.cend());
			}

            /**
             * Converts to a std::string, by using base64 encoding.
             */
			operator std::string() const
			{
                std::string b64 = Base64::encode(ByteArray(m_data.cbegin(), m_data.cend()));
			    std::replace(b64.begin(), b64.end(), '+', '-');
			    std::replace(b64.begin(), b64.end(), '/', '~');

			    return b64;
			}

		private:
			std::array<value_type, L> m_data;
	};

	template<unsigned int L>
	std::ostream& operator<<(std::ostream &s, StaticByteArray<L> const &sba)
	{
		s << std::string(sba);
		return s;
	}
}

namespace std {
	template<unsigned int L>
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
