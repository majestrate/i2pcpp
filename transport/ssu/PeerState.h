#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include <boost/asio.hpp>

#include "../../datatypes/RouterIdentity.h"
#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"

#include "../../Log.h"

#include "InboundMessageState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				PeerState(Endpoint const &ep, RouterHash const &rh);

				SessionKey getCurrentSessionKey() const;
				SessionKey getCurrentMacKey() const;
				SessionKey getNextSessionKey() const;
				SessionKey getNextMacKey() const;

				void setCurrentSessionKey(SessionKey const &sk);
				void setCurrentMacKey(SessionKey const &mk);
				void setNextSessionKey(SessionKey const &sk);
				void setNextMacKey(SessionKey const &mk);

				RouterHash getHash() const;
				Endpoint getEndpoint() const;

			private:
				Endpoint m_endpoint;
				RouterHash m_routerHash;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;
		};

		typedef std::shared_ptr<PeerState> PeerStatePtr;
	}
}

#endif
