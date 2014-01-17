#ifndef FORMATTINGERROR_H
#define FORMATTINGERROR_H

#include <stdexcept>

namespace i2pcpp {
    class FormattingError : public std::runtime_error {
        public:
            FormattingError();
    };
}

#endif
