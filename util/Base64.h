#ifndef BASE64_H
#define BASE64_H

#include <string>

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
    namespace Base64 {
        ByteArray decode(std::string const &s);

        std::string encode(unsigned char *data, size_t length);
        template <typename T>
        std::string encode(T x)
        {
            return encode(x.data(), x.size());
        }

        bool is_base64(unsigned char c);
    }
}

#endif
