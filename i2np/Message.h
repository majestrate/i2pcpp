#ifndef I2NPMESSAGE_H
#define I2NPMESSAGE_H

namespace i2pcpp {
	namespace I2NP {
		class Message {
			public:
			private:
				unsigned char m_type;
				unsigned int m_expiration;
		};
	}
}

#endif
