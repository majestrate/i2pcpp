#ifndef TUNNELMESSAGEFRAGMENTS_H
#define TUNNELMESSAGEFRAGMENTS_H

#include <forward_list>

#include "../datatypes/StaticByteArray.h"
#include "../datatypes/ByteArray.h"
#include "../i2np/Message.h"

#include "DeliveryInstructions.h"

namespace i2pcpp {
	class TunnelMessageFragments {
		public:
			TunnelMessageFragments();
			TunnelMessageFragments(uint32_t const tunnelId, I2NP::Message const &msg);

		private:
			uint32_t m_tunnelId;

			StaticByteArray<16, true> m_iv;
			StaticByteArray<4> m_checksum;
			std::forward_list<std::pair<DeliveryInstructions, ByteArray>> m_fragments;
	};
}

#endif
