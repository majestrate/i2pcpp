/**
 * @file RouterHash.h
 * @brief Defines the i2pcpp::RouterHash typedef.
 */
#ifndef ROUTERHASH_H
#define ROUTERHASH_H

#include "StaticByteArray.h"

namespace i2pcpp {
    typedef StaticByteArray<32> RouterHash;

    template<typename T>
    inline RouterHash toRouterHash(T const &t)
    {
        return toStaticByteArray<32>(t);
    }
}

#endif
