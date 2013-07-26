#include "TunnelMessage.h"

namespace i2pcpp {
	TunnelMessage::TunnelMessage(std::array<unsigned char, 1024> const &data)
	{
		std::copy(data.cbegin(), data.cbegin() + 16, m_iv.begin());
		std::copy(data.cbegin() + 16, data.cbegin() + 16 + 1008, m_data.begin());
	}
}
