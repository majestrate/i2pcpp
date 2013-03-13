#ifndef I2NPTUNNELGATEWAY_H
#define I2NPTUNNELGATEWAY_H

#include <list>

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class TunnelGateway : public Message {
			public:
				TunnelGateway() {}
				TunnelGateway(uint32_t tunnelId, ByteArray const &data) : m_tunnelId(tunnelId), m_data(data) {}

				Message::Type getType() const { return Message::Type::TUNNEL_GATEWAY; }

				uint32_t getTunnelId() const { return m_tunnelId; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArray::const_iterator &dataItr);

			private:
				uint32_t m_tunnelId;
				ByteArray m_data;
		};
	}
}

#endif
