#include "TunnelMessageFragments.h"

namespace i2pcpp {
	TunnelMessageFragments::TunnelMessageFragments() {}

	TunnelMessageFragments::TunnelMessageFragments(uint32_t const tunnelId, I2NP::Message const &msg) :
		m_tunnelId(tunnelId)
	{
	}
}
