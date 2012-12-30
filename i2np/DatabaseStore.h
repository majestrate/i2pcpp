#ifndef I2NPDATABASESTORE_H
#define I2NPDATABASESTORE_H

#include <array>

#include "../datatypes/RouterHash.h"
#include "../datatypes/ByteArray.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseStore : public Message {
			public:
				Message::Type getType() const { return Message::Type::DB_STORE; }

				enum Type {
					ROUTER_INFO = 0,
					LEASE_SET = 1
				};

				Type getStoreType() const { return m_type; }
				unsigned long getReplyToken() const { return m_replyToken; }
				const ByteArray& getData() const { return m_data; }

			protected:
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				std::array<unsigned char, 32> m_key;
				Type m_type;
				unsigned long m_replyToken;
				unsigned long m_replyTunnelId;
				RouterHash m_replyGateway;
				ByteArray m_data;
		};
	}
}

#endif
