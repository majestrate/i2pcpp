#ifndef I2NPDATABASESTORE_H
#define I2NPDATABASESTORE_H

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseStore : public Message {
			public:
				enum Type {
					RouterInfo,
					LeaseSet
				}
			private:
				Type m_type;
				ByteArray m_data;
		};
	}
}

#endif
