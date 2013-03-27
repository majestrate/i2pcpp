#ifndef I2NPDATABASESEARCHREPLY_H
#define I2NPDATABASESEARCHREPLY_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseSearchReply : public Message {
			public:
				DatabaseSearchReply();

				Message::Type getType() const;

				const std::array<unsigned char, 32>& getKey() const;
				const std::list<RouterHash>& getHashes() const;
				const RouterHash& getFrom() const;

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				std::array<unsigned char, 32> m_key;
				std::list<RouterHash> m_hashes;
				RouterHash m_from;
		};
	}
}

#endif
