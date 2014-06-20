/**
 * @file Destination.h
 * @brief Defines the i2pcpp::Destination typedef.
 */
#ifndef DESTINATION_H
#define DESTINATION_H

#include "StaticByteArray.h"

namespace i2pcpp {
    typedef StaticByteArray<32> Destination;

    template<typename T>
    inline Destination toDestination(T const &t)
    {
        return toStaticByteArray<32>(t);
    }
}

#endif
