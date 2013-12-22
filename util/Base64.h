#ifndef BASE64_H
#define BASE64_H

#include <string>

#include "../datatypes/ByteArray.h"

namespace i2pcpp {

    class Base64 {
        public:
            static ByteArray decode(std::string const &s);
            static std::string encode(ByteArray const &b);

        private:
            static bool is_base64(unsigned char c);
            static const std::string b64chars;
    };
}

#endif
