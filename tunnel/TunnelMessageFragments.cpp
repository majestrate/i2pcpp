#include "TunnelMessageFragments.h"

namespace i2pcpp {
	TunnelMessageFragments::TunnelMessageFragments() {}

	TunnelMessageFragments::TunnelMessageFragments(uint32_t const tunnelId, ByteArray const &data) :
		m_tunnelId(tunnelId)
	{
	}
}
