#ifndef I2NPDATABASESEARCHREPLY_H
#define I2NPDATABASESEARCHREPLY_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseSearchReply : public Message {
			public:
				DatabaseSearchReply() {}

				Message::Type getType() const { return Message::Type::DB_SEARCH_REPLY; }

				const std::array<unsigned char, 32>& getKey() const { return m_key; }
				const std::list<RouterHash>& getHashes() const { return m_hashes; }
				const RouterHash& getFrom() const { return m_from; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				std::array<unsigned char, 32> m_key;
				std::list<RouterHash> m_hashes;
				RouterHash m_from;
		};
	}
}

#endif
