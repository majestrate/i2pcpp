#ifndef I2NPDATABASELOOKUP_H
#define I2NPDATABASELOOKUP_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseLookup : public Message {
			public:
				DatabaseLookup() {}
				DatabaseLookup(std::array<unsigned char, 32> const &key, RouterHash const &from, uint32_t sendReplyTo, std::list<RouterHash> excludedPeers = std::list<RouterHash>()) : m_key(key), m_from(from), m_sendReplyTo(sendReplyTo), m_excludedPeers(excludedPeers) {}

				Message::Type getType() const { return Message::Type::DB_LOOKUP; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				std::array<unsigned char, 32> m_key;
				RouterHash m_from;
				uint32_t m_sendReplyTo;
				std::list<RouterHash> m_excludedPeers;
		};
	}
}

#endif
