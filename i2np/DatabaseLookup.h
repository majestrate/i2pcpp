#ifndef I2NPDATABASELOOKUP_H
#define I2NPDATABASELOOKUP_H

#include <list>

#include "../datatypes/RouterHash.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseLookup : public Message {
			public:
				DatabaseLookup(StaticByteArray<32> const &key, RouterHash const &from, uint32_t sendReplyTo, std::list<RouterHash> excludedPeers = std::list<RouterHash>());

				static DatabaseLookup parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			protected:
				DatabaseLookup() = default;

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
