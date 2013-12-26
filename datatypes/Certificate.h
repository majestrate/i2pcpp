#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "Datatype.h"

namespace i2pcpp {
    class Certificate : public Datatype {
        public:
            enum class Type {
                NIL,
                HASHCASH,
                HIDDEN,
                SIGNED,
                MULTIPLE
            };

            Certificate();
            Certificate(ByteArrayConstItr &begin, ByteArrayConstItr end);

            uint16_t getLength() const;
            ByteArray serialize() const;

        private:
            Type m_type;
            ByteArray m_payload;
    };
}

#endif
