#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		const RouterIdentity& PeerState::getIdentity() const
		{
			return m_identity;
		}

		const Endpoint& PeerState::getEndpoint() const
		{
			return m_endpoint;
		}
	}
}
