#ifndef I2NPGARLIC_H
#define I2NPGARLIC_H

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        class Garlic : public Message {
            public:
                static Garlic parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                Garlic() = default;

                ByteArray compile() const;
        };
    }
}

#endif
