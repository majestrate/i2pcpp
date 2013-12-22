/**
 * @file ByteArray.h
 * @brief Defines the i2pcpp::ByteArray type, as well as a number of useful
 *  typedefs and operators.
 */
#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <stdint.h>

#include <memory>
#include <vector>

namespace i2pcpp {
    /**
     * An i2pcpp::ByteArray is a std::vector<unisgned char>.
     */
    typedef std::vector<unsigned char> ByteArray;
    typedef std::shared_ptr<ByteArray> ByteArrayPtr;
    typedef ByteArray::iterator ByteArrayItr;
    typedef ByteArray::const_iterator ByteArrayConstItr;
}

namespace std {
    std::ostream& operator<<(std::ostream &s, i2pcpp::ByteArray const &data);
}

#endif
