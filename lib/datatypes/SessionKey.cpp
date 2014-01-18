#include <i2pcpp/datatypes/SessionKey.h>

namespace i2pcpp {
    SessionKey toSessionKey(ByteArray const &b)
    {
        return toStaticByteArray<32>(b);
    }
}
