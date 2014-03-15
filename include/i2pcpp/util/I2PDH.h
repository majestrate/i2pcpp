#ifndef I2PDH_H
#define I2PDH_H

namespace Botan { class DL_Group; }

namespace i2pcpp {
    class DH {
        public:
            static const Botan::DL_Group getGroup();
    };
}

#endif
