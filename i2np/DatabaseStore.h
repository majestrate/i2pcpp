#ifndef I2NPDATABASESTORE_H
#define I2NPDATABASESTORE_H

#include <array>

#include "../datatypes/RouterHash.h"
#include "../datatypes/StaticByteArray.h"
#include "../datatypes/ByteArray.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class DatabaseStore : public Message {
			public:
				enum DataType {
					ROUTER_INFO = 0,
					LEASE_SET = 1
				};

				DatabaseStore();
				DatabaseStore(StaticByteArray<32> const &key, DataType type, uint32_t replyToken, ByteArray const &data);

				Message::Type getType() const;

				DataType getDataType() const;
				uint32_t getReplyToken() const;
				const ByteArray& getData() const;

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				StaticByteArray<32> m_key;
				DataType m_type;
				uint32_t m_replyToken;
				uint32_t m_replyTunnelId;
				RouterHash m_replyGateway;
				ByteArray m_data;
		};
	}
}

#endif
