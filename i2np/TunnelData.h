#ifndef I2NPTUNNELDATA_H
#define I2NPTUNNELDATA_H

#include <array>

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class TunnelData : public Message {
			public:
				TunnelData() {}
				TunnelData(uint32_t const tunnelId, ByteArray const &data);

				Message::Type getType() const { return Message::Type::TUNNEL_DATA; }

				uint32_t getTunnelId() const { return m_tunnelId; }
				const std::array<unsigned char, 1024>& getData() const { return m_data; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				uint32_t m_tunnelId;
				std::array<unsigned char, 1024> m_data;
		};
	}
}

#endif
