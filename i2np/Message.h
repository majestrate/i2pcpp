#ifndef I2NPMESSAGE_H
#define I2NPMESSAGE_H

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace I2NP {
		class Message {
			public:
				virtual ~Message() {}

				enum MessageType {
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

				static Message *fromBytes(ByteArray const &data);
				virtual MessageType getType() = 0;

			protected:
				virtual bool parse(ByteArray::const_iterator &dataItr) = 0;

				unsigned long m_expiration;
		};
	}
}

#endif
