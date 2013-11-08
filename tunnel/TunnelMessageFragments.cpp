#include "TunnelMessageFragments.h"

namespace i2pcpp {
	TunnelMessageFragments::TunnelMessageFragments() {}

	TunnelMessageFragments::TunnelMessageFragments(TunnelMessageFragments::DeliveryType const t, uint32_t const tunnelId, RouterHash const &toHash, I2NP::MessagePtr const &msg) :
		m_type(t),
		m_tunnelId(tunnelId),
		m_toHash(toHash)
	{
		ByteArray data = msg->toBytes();

		int headerSize = 0;

		headerSize++; // flag

		if(m_type == DeliveryType::TUNNEL)
			headerSize += 4; // tunnelId

		headerSize += 32; // toHash

		// Delay and extended options not implemented

		headerSize += 2; // size

		if(data.size() > (1003 - headerSize)) {
			// Too big, can't do it all in one fragment
			headerSize += 4; // msgId
		}

		ByteArray initial(headerSize);

		unsigned char flag = 0x00;
		flag |= (unsigned char)m_type << 5;
	}
}
