#include <i2pcpp/datatypes/LeaseSet.h>

namespace i2pcpp {
    LeaseSet::LeaseSet(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
    }

    LeaseSet::LeaseSet(Destination const &dst, ByteArray const &encKey, ByteArray const &sigKey)
    {
    }

    ByteArray LeaseSet::serialize() const
    {
        return ByteArray();
    }
}
