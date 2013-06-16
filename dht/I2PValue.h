#ifndef DHTI2PVALUE_H
#define DHTI2PVALUE_H

#include "../datatypes/RouterInfo.h"

namespace i2pcpp {
	namespace DHT {
		enum I2PValueType {
			ROUTER_INFO,
			LEASESET
		};

		struct I2PValue {
			I2PValueType type;

			union {
				RouterInfo ri;
			};
		};
	}
}

#endif
