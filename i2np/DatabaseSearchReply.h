/**
 * @file DatabaseSearchReply.h
 * @brief Defines the i2pcpp::I2NP::DatabaeSearchReply message type.
 */
#ifndef I2NPDATABASESEARCHREPLY_H
#define I2NPDATABASESEARCHREPLY_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        
        /**
         * Represents a negative response to an i2pcpp::DatabaseLookup search
         *  message. This message type is used for failed database responses.
         * Rather than returning the value associated with the key, a list of
         *  router hashes closest to the requested key are returned (typically 3).
         */
        class DatabaseSearchReply : public Message {
            public:
                /**
                 * @return the key of the object that was searched for
                 */
                const StaticByteArray<32>& getKey() const;

                /**
                 * @return a list of typically 3 i2pcpp::RouterHash objects,
                 *  closest to the key
                 */
                const std::list<RouterHash>& getHashes() const;

                /**
                 * @return the i2pcpp::RouterHash of the sending router.
                 * @warning this is unauthenticated, thus it should not be
                 *  trusted
                 */
                const RouterHash& getFrom() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::DatabaseSearchReply
                 *  object.
                 * The format to be parsed is the 32B key, followed by a 1B
                 *  integer (size), followed by \a size 32B i2pcpp::RouterHash
                 *  objects and an i2pcpp::RouterHash (from field).
                 */
                static DatabaseSearchReply parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                DatabaseSearchReply() = default;

                /**
                 * Puts the 32B key, the 1B size integer, the \a size
                 *  i2pcpp::RouterHash objects and the from i2pcpp::RouterHash
                 *  in an i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                StaticByteArray<32> m_key;
                std::list<RouterHash> m_hashes;
                RouterHash m_from;
        };
    }
}

#endif
