#include "RecordNotFound.h"

#include <string>

namespace i2pcpp {
    RecordNotFound::RecordNotFound(std::string const &s) :
        std::runtime_error(std::string("Record not found: ") + s) {}
}
