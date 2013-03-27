#ifndef I2NPDATABASELOOKUP_H
#define I2NPDATABASELOOKUP_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseLookup : public Message {
			public:
				DatabaseLookup();
				DatabaseLookup(std::array<unsigned char, 32> const &key, RouterHash const &from, uint32_t sendReplyTo, std::list<RouterHash> excludedPeers = std::list<RouterHash>());

				Message::Type getType() const;

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				std::array<unsigned char, 32> m_key;
				RouterHash m_from;
				uint32_t m_sendReplyTo;
				std::list<RouterHash> m_excludedPeers;
		};
	}
}

#endif
