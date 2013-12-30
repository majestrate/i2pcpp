#include "RouterHash.h"

namespace i2pcpp {
    RouterHash toRouterHash(ByteArray const &b)
    {
        return toStaticByteArray<32>(b);
    }
}
