#ifndef I2NPMESSAGE_H
#define I2NPMESSAGE_H

#include "../datatypes/ByteArray.h"
#include "../datatypes/Date.h"

namespace i2pcpp {
	namespace I2NP {
		class Message {
			public:
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

				static void handleMessage(ByteArray::const_iterator &dataItr);

			private:
				static void handleDeliveryStatus(ByteArray::const_iterator &dataItr);

				MessageType m_type;
		};
	}
}

#endif
