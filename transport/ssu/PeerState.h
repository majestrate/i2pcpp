#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include "../../datatypes/RouterIdentity.h"
#include "../../datatypes/Endpoint.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				const RouterIdentity& getIdentity() const;
				const Endpoint& getEndpoint() const;

			private:
				RouterIdentity m_identity;
				Endpoint m_endpoint;
		};

		typedef std::shared_ptr<PeerState> PeerStatePtr;
	}
}

template<>
struct std::hash<i2pcpp::SSU::PeerStatePtr> {
	public:
		size_t operator()(const i2pcpp::SSU::PeerStatePtr &ps) const
		{
			std::hash<i2pcpp::RouterHash> f;
			return f(ps->getIdentity().getHash());
		}
};

#endif
