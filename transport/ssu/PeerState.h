#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include "../../datatypes/RouterIdentity.h"
#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				PeerState(Endpoint const &ep, RouterIdentity const &ri);

				SessionKey getCurrentSessionKey() const;
				SessionKey getCurrentMacKey() const;
				SessionKey getNextSessionKey() const;
				SessionKey getNextMacKey() const;

				void setCurrentSessionKey(SessionKey const &sk);
				void setCurrentMacKey(SessionKey const &mk);
				void setNextSessionKey(SessionKey const &sk);
				void setNextMacKey(SessionKey const &mk);

				const RouterIdentity& getIdentity() const;
				const Endpoint& getEndpoint() const;

			private:
				Endpoint m_endpoint;
				RouterIdentity m_identity;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;

				mutable std::mutex m_mutex;
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
