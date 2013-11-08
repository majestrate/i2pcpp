#ifndef TUNNELMESSAGEFRAGMENTS_H
#define TUNNELMESSAGEFRAGMENTS_H

#include "../datatypes/RouterHash.h"
#include "../datatypes/StaticByteArray.h"
#include "../datatypes/ByteArray.h"
#include "../i2np/Message.h"

namespace i2pcpp {
	class TunnelMessageFragments {
		public:
			enum class DeliveryType {
				TUNNEL = 0x01,
				ROUTER = 0x02
			};

			TunnelMessageFragments();
			TunnelMessageFragments(DeliveryType const t, uint32_t const tunnelId, RouterHash const &toHash, I2NP::MessagePtr const &msg);

		private:
			DeliveryType m_type;
			uint32_t m_tunnelId;
			RouterHash m_toHash;

			std::vector<ByteArrayPtr> m_fragments;
	};
}

#endif
