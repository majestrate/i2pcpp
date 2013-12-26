#ifndef RECORDNOTFOUND_H
#define RECORDNOTFOUND_H

#include <stdexcept>

namespace i2pcpp {
    class RecordNotFound : public std::runtime_error {
        public:
            RecordNotFound(std::string const &s);
    };
}

#endif
