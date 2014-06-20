#ifndef LEASESET_H
#define LEASESET_H

#include "Datatype.h"

namespace i2pcpp {
    class LeaseSet : public Datatype {
        public:
            LeaseSet(ByteArrayConstItr &begin, ByteArrayConstItr end);
            LeaseSet(Destination const &dst, ByteArray const &encKey, ByteArray const &sigKey);
            virtual ByteArray serialize() const;

    };
}

#endif
