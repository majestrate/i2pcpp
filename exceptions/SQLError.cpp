#include "SQLError.h"

#include <string>

namespace i2pcpp {
    SQLError::SQLError(const std::string &statement) :
        std::runtime_error(std::string("SQL error when executing: " + statement)) {}
}
