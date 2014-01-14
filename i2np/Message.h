/**
 * @file Message.h
 * @brief Defines the i2pcpp::I2NP::Message base class.
 */
#ifndef I2NPMESSAGE_H
#define I2NPMESSAGE_H

#include <memory>

#include "../datatypes/ByteArray.h"
#include "../datatypes/Date.h"

namespace i2pcpp {
    namespace I2NP {

        /**
         * Abstract base class that represents the common properties of all I2NP
         *  messages.
         */
        class Message {
            public:
                /**
                 * Strong enum that stores the different message types.
                 * These range from 1 to 24, but not all numbers in that range
                 *  are used.
                 * This is 1B integer.
                 */
                enum class Type {
                    DB_STORE = 1,
                    DB_LOOKUP = 2,
                    DB_SEARCH_REPLY = 3,
                    DELIVERY_STATUS = 10,
                    GARLIC = 11,
                    TUNNEL_DATA = 18,
                    TUNNEL_GATEWAY = 19,
                    DATA = 20,
                    TUNNEL_BUILD = 21,
                    TUNNEL_BUILD_REPLY = 22,
                    VARIABLE_TUNNEL_BUILD = 23,
                    VARIABLE_TUNNEL_BUILD_REPLY = 24
                };

                virtual ~Message() {}

                /**
                 * Converts the i2pcpp::I2NP::Message to an i2pcpp::ByteArray.
                 * That is, serializes it.
                 * The format of the standard header is:
                 * type (1B), msg-id (4B), expiration (Date:8B), size (2B),
                 *  checksum (1B), data (size B)
                 * The format of the short header is:
                 * type (1B), short expiration (Seconds since epoch:4B)
                 * @param standardHeader if set to true, the long standard header is used, otherwise
                 *  the short header is used (as is the case for SSU)
                 */
                ByteArray toBytes(bool standardHeader = true) const;

                /**
                 * @return the 4 byte message identifier
                 */
                uint32_t getMsgId() const;

                /**
                 * @return the type of the message
                 * @throw std::runtime_error if the type could not be obtained
                 *  (i.e. the derived type is unkown)
                 */
                Type getType() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::Message object.
                 * That is, deserializes.
                 * @param msgId the message identifier
                 * @param data the bytes representing the message
                 * @param standardHeader if set to true, the long standard header is used, otherwise
                 *  the short header is used (as is the case for SSU)
                 * @return a pointer to the newly created i2pcpp::I2NP::Message
                 */
                static std::shared_ptr<Message> fromBytes(uint32_t msgId, ByteArray const &data, bool standardHeader = true);

            protected:
                /**
                 * Default constructs. Generates a random message identifier.
                 */
                Message();

                /**
                 * Constructs from a given message identifier.
                 */
                Message(uint32_t msgId);

                /**
                 * Converts the whole message (excluding the header) to an
                 *  i2pcpp::ByteArray. This is used in serialization.
                 * @see i2pcpp::I2NP::Message::toBytes
                 */
                virtual ByteArray compile() const = 0;

                uint32_t m_msgId; ///< The message identifier
                uint32_t m_expiration; ///< Short expiration data in seconds
                Date m_longExpiration; ///< Long expiration date in miliseconds
        };

        typedef std::shared_ptr<Message> MessagePtr;
    }
}

#endif
