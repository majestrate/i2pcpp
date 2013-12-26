#include "StatementPrepareError.h"

namespace i2pcpp {
    StatementPrepareError::StatementPrepareError() :
        std::runtime_error("Could not prepare SQL statement") {}
}
