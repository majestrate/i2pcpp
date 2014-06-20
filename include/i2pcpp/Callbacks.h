#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <i2pcpp/datatypes/Destination.h>
#include <i2pcpp/datatypes/LeaseSet.h>

namespace i2pcpp {
    struct Callbacks {
        std::function<void(const Destination, const LeaseSet)> querySuccess;
        std::function<void(const Destination)> queryFail;
    };
}

#endif
