#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

namespace i2pcpp {
    namespace Base64 {
        bool is_base64(unsigned char c);

        std::string encode(unsigned char *data, size_t length);
        template <typename T>
        std::string encode(T x)
        {
            return encode(x.data(), x.size());
        }

        std::vector<unsigned char> decode(std::string const &s);
    }
}

#endif
