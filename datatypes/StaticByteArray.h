#ifndef STATICBYTEARRAY_H
#define STATICBYTEARRAY_H

#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>


#include <boost/functional/hash.hpp>

#include "ByteArray.h"
#include "../util/Base64.h"

namespace i2pcpp {

    template<std::size_t L>
    using StaticByteArray = std::array<unsigned char, L>;

    template<std::size_t L>
    StaticByteArray<L> toStaticByteArray(ByteArray const &b)
    {
        StaticByteArray<L> ret;

        if(b.size() < L)
            std::copy(b.cbegin(), b.cend(), ret.begin());
        else
            std::copy(b.cbegin(), b.cbegin() + L, ret.begin());

        return ret;
    }

    template<std::size_t L>
    ByteArray toByteArray(StaticByteArray<L> const &sba)
    {
        return ByteArray(sba.cbegin(), sba.cend());
    }

    template<std::size_t L>
    std::size_t hash_value(StaticByteArray<L> const &sba)
    {
        boost::hash<std::string> f;
        return f(sba);
    }
}

namespace std {
    template<std::size_t L>
    struct hash<i2pcpp::StaticByteArray<L>> {
        size_t operator()(const i2pcpp::StaticByteArray<L> &sba) const
        {
            hash<string> f;
            return f(std::string(sba.cbegin(), sba.cend()));
        }
    };

    template<std::size_t L>
    std::ostream& operator<<(std::ostream &s, i2pcpp::StaticByteArray<L> const &sba)
    {
        s << i2pcpp::Base64::encode(sba);
        return s;
    }
}

#endif
