#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <stdint.h>

#include <memory>
#include <vector>

namespace i2pcpp {
    typedef std::vector<unsigned char> ByteArray;
    typedef std::shared_ptr<ByteArray> ByteArrayPtr;
    typedef ByteArray::iterator ByteArrayItr;
    typedef ByteArray::const_iterator ByteArrayConstItr;
}

namespace std {
    std::ostream& operator<<(std::ostream &s, i2pcpp::ByteArray const &data);
}

#endif
