/**
 * @file DeliveryStatus.h
 * @brief Defines the i2pcpp::I2NP::DeliveryStatus message type.
 */
#ifndef I2NPDELIVERYSTATUS_H
#define I2NPDELIVERYSTATUS_H

#include "Message.h"

#include <i2pcpp/datatypes/Date.h>

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a delivery status message, used for message acknowledgment.
         * They are also used for testing tunnels.
         * These messages are usually created by the message originator, to be
         *  returned by the destination. They are then wrapped in an
         *  i2pcpp::I2NP::Garlic message, together with the actual message.
         */
        class DeliveryStatus : public Message {
            public:
                /**
                 * Constructs from a \a msgId and an i2pcpp::Date object.
                 * @param timestamp the time the message was succesfully created
                 *  or delivered
                 */
                DeliveryStatus(uint32_t msgId, Date timestamp);

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::DeliveryStatus
                 *  object.
                 * The format to be parsed is a 4B message id followed by an 8B
                 *  i2pcpp::Date.
                 */
                static DeliveryStatus parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                DeliveryStatus() = default;

                /**
                 * Puts the 4B message identifier, followed by the 8B i2pcpp::Date
                 *  in an i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                uint32_t m_msgId;
                Date m_timestamp;
        };
    }
}

#endif
