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

    template <typename T>
    inline ByteArray toByteArray(T const &t)
    {
        return ByteArray(t.cbegin(), t.cend());
    }

    template <typename T>
    inline uint32_t parseUint32(T &itr)
    {
        uint32_t x = (itr[0] << 24) | (itr[1] << 16) | (itr[2] << 8) | (itr[3]);
        itr += 4;

        return x;
    }

    template <typename T>
    inline uint16_t parseUint16(T &itr)
    {
        uint16_t x = (itr[0] << 8) | (itr[1]);
        itr += 2;

        return x;
    }
}

namespace std {
    std::ostream& operator<<(std::ostream &s, i2pcpp::ByteArray const &data);
    std::istream& operator>>(std::istream &s, i2pcpp::ByteArray &data);
}

#endif
