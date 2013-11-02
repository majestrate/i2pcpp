#include "Message.h"

#include <botan/auto_rng.h>

#include "../exceptions/FormattingError.h"

#include "DeliveryStatus.h"
#include "DatabaseStore.h"
#include "DatabaseSearchReply.h"
#include "VariableTunnelBuild.h"
#include "VariableTunnelBuildReply.h"
#include "TunnelData.h"
#include "TunnelGateway.h"

namespace i2pcpp {
	namespace I2NP {
		MessagePtr Message::fromBytes(uint32_t const msgId, ByteArray const &data, bool standardHeader)
		{
			MessagePtr m;

			auto dataItr = data.cbegin();
			Type mtype = (Type)*(dataItr++);

			switch(mtype)
			{
				case Type::DELIVERY_STATUS:
					m = std::make_shared<DeliveryStatus>();
					break;

				case Type::DB_STORE:
					m = std::make_shared<DatabaseStore>();
					break;

				case Type::DB_SEARCH_REPLY:
					m = std::make_shared<DatabaseSearchReply>();
					break;

				case Type::VARIABLE_TUNNEL_BUILD:
					m = std::make_shared<VariableTunnelBuild>();
					break;

				case Type::VARIABLE_TUNNEL_BUILD_REPLY:
					m = std::make_shared<VariableTunnelBuildReply>();
					break;

				case Type::TUNNEL_DATA:
					m = std::make_shared<TunnelData>();
					break;

				case Type::TUNNEL_GATEWAY:
					m = std::make_shared<TunnelGateway>();
					break;

				default:
					return MessagePtr();
			}

			m->m_msgId = msgId;

			if(standardHeader) {
				m->m_msgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
				m->m_longExpiration = Date(dataItr, data.cend());

				uint16_t size = (*(dataItr++) << 8) | *(dataItr++);
				uint8_t checksum = *dataItr++; // TODO verify this

				if(data.cend() - dataItr != size)
					throw FormattingError();
			} else {
				m->m_expiration = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
			}

			if(m->parse(dataItr, data.cend()))
				return m;
			else
				return MessagePtr();
		}

		Message::Message()
		{
			Botan::AutoSeeded_RNG rng;

			rng.randomize((unsigned char *)&m_msgId, sizeof(m_msgId));
		}

		Message::Message(uint32_t msgId) :
			m_msgId(msgId) {}

		uint32_t Message::getMsgId() const
		{
			return m_msgId;
		}

		ByteArray Message::toBytes() const
		{
			ByteArray b;
			const ByteArray& m = getBytes();

			b.insert(b.end(), getType());

			// m_expiration?
			uint32_t expiration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 60;
			b.insert(b.end(), expiration >> 24);
			b.insert(b.end(), expiration >> 16);
			b.insert(b.end(), expiration >> 8);
			b.insert(b.end(), expiration);

			b.insert(b.end(), m.cbegin(), m.cend());

			return b;
		}
	}
}
