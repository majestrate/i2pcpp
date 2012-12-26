#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include "../datatypes/Endpoint.h"
#include "../datatypes/SessionKey.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				PeerState(Endpoint const &ep, bool isInbound) : m_endpoint(ep), m_isInbound(isInbound) {}

			private:
				Endpoint m_endpoint;
				bool m_isInbound;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;
		};
	}
}

#endif
