/**
 * @file DatabaseLookup.h
 * @brief Defines the i2pcpp::I2NP::DatabaseLookup message type.
 */
#ifndef I2NPDATABASELOOKUP_H
#define I2NPDATABASELOOKUP_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        /**
         * Represents a database lookup message. These messages are used to
         *  lookup an item in the netDB.
         * @see i2pcpp::I2NP::DatabaseStore
         * @see i2pcpp::I2NP::DatabaseSearchReply
         */
        class DatabaseLookup : public Message {
            public:
                /**
                 * Constructs from a \a key, hash of the RI who is asking (or of
                 *  the IBGW to send the reply to), the tunnel ID of the tunnel
                 *  to send the reply to and a list of excluded peers.
                 * @param key the 32B key (SHA-256 hash of object to lookup) to lookup
                 * @param from the 32B i2pcpp::RouterHash of the sending router,
                 *  this is the router to whom the reply must be sent or the hash
                 *  of the reply tunnel gateway
                 * @param sendReplyTo the tunnel id to send the reply to, if
                 *  it is set to 0 the deliveryFlag will be set to 0x00.
                 * @param excludedPeers a list of peers not to be included in the
                 *  list that is returned on a failed lookup, if it includes an
                 *  i2pcpp::RouterHash of all zeroes, only non-floodfill routers
                 *  should be included in that list
                 */
               DatabaseLookup(StaticByteArray<32> const &key, RouterHash const &from, uint32_t sendReplyTo, std::list<RouterHash> excludedPeers = std::list<RouterHash>());

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::DatabaseLookup object.
                 * The format to be parsed is 32B key, followed by the 32B
                 *  i2pcpp::RouterHash of the sending router, followed by 1B of flags,
                 *  followed by a 4B tunnel id, followed by 2B size integer, followed by
                 *  \a size i2pcpp::RouterHash objects.
                 * If the encryption flag is specified, this is followed by a 32B
                 *  session key for AES-256, a 1B size integer, followed by \a size
                 *  session tags.
                 * @todo implement this (needed for floodfill mode)
                 */
               static DatabaseLookup parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                DatabaseLookup() = default;

                /**
                 * Puts the 32B key, the i2pcpp::RouterHash of the sender, the
                 *  1B flags, the 4B tunnel id, the 2B size integer, \a size
                 *  i2pcpp::RouterHash objects into an i2pcpp::ByteArray.
                 * @todo implement encrypionn, for when the encryption flag is set
                 */
                ByteArray compile() const;

            private:
                StaticByteArray<32> m_key;
                RouterHash m_from;
                uint32_t m_sendReplyTo;
                std::list<RouterHash> m_excludedPeers;
        };
    }
}

#endif
