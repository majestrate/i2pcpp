/**
 * @file Garlic.h
 * @brief Defines the i2pcpp::I2NP::Garlic message type.
 */
#ifndef I2NPGARLIC_H
#define I2NPGARLIC_H

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a garlic message. These are message that wrap multiple
         *  i2pcpp::I2NP::Message objects. After decrypting, they consist of
         *  multiple garlic cloves, containing the I2NP messages.
         * @todo actually implement this
         */
        class Garlic : public Message {
            public:
                /*
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::Garlic object.
                 * @todo implement this
                 */
                static Garlic parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                Garlic() = default;

                ByteArray compile() const;
        };
    }
}

#endif
