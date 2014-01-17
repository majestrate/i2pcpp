#ifndef SQLERROR_H
#define SQLERROR_H

#include <stdexcept>

namespace i2pcpp {
    class SQLError : public std::runtime_error {
        public:
            SQLError(std::string const &statement);
    };
}

#endif
