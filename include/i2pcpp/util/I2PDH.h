#ifndef I2PDH_H
#define I2PDH_H

#include <botan/dl_group.h>

namespace i2pcpp {
    class DH {
        public:
            static const Botan::DL_Group getGroup();
    };
}

#endif
