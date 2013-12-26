#include "ByteArray.h"

#include <iomanip>
#include <iostream>

namespace std {
    std::ostream& operator<<(std::ostream &s, i2pcpp::ByteArray const &data)
    {
        for(auto c: data) s << std::setw(2) << std::setfill('0') << std::hex << (int)c << std::setw(0) << std::dec;

        return s;
    }
}
