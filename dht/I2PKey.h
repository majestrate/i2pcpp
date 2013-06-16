#ifndef DHTI2PKEY_H
#define DHTI2PKEY_H

#include "../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace DHT {
		enum I2PKeyType {
			ROUTER_HASH,
			DESTINATION
		};

		struct I2PKey {
			I2PKeyType type;

			union {
				RouterHash rh;
			};
		};
	}
}

namespace std {
	template<>
	struct hash<i2pcpp::DHT::I2PKey> {
		public:
			size_t operator()(const i2pcpp::DHT::I2PKey &k) const
			{
				switch(k.type) {
					case i2pcpp::DHT::ROUTER_HASH:
						{
							hash<i2pcpp::RouterHash> f;
							return f(k.rh);
						}

					case i2pcpp::DHT::DESTINATION:
						break;
				}

				return 0;
			}
	};
}

#endif
