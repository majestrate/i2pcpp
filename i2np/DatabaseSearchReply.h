#ifndef I2NPDATABASESEARCHREPLY_H
#define I2NPDATABASESEARCHREPLY_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {
        class DatabaseSearchReply : public Message {
            public:
                const StaticByteArray<32>& getKey() const;
                const std::list<RouterHash>& getHashes() const;
                const RouterHash& getFrom() const;

                static DatabaseSearchReply parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                DatabaseSearchReply() = default;

                ByteArray compile() const;

            private:
                StaticByteArray<32> m_key;
                std::list<RouterHash> m_hashes;
                RouterHash m_from;
        };
    }
}

#endif
