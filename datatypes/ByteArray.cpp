/**
 * @file ByteArray.cpp
 * @brief Implements ByteArray.h.
 */
#include "ByteArray.h"

#include <iomanip>
#include <iostream>

#include <boost/algorithm/hex.hpp>

namespace std {
    std::ostream& operator<<(std::ostream &s, i2pcpp::ByteArray const &data)
    {
        for(auto c: data)
            s << std::setw(2) << std::setfill('0') << std::hex << (int)c << std::setw(0) << std::dec;

        return s;
    }

    std::istream& operator>>(std::istream &s, i2pcpp::ByteArray &data)
    {
        std::string in(static_cast<std::stringstream const&>(std::stringstream() << s.rdbuf()).str());
        boost::algorithm::unhex (in, std::back_inserter(data));
        return s;
    };
}
