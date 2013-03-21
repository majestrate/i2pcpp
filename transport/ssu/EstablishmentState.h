#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include <botan/dh.h>
#include <botan/dsa.h>

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"
#include "../../datatypes/RouterIdentity.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(Botan::DL_Group const &params, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk);
				EstablishmentState(Botan::DL_Group const &params, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri);
				EstablishmentState(EstablishmentState const &state) = delete;
				~EstablishmentState();

				enum Direction {
					INBOUND,
					OUTBOUND
				};

				Direction getDirection() const;

			private:
				Direction m_direction;
				Botan::DH_PrivateKey *m_dhKey;

				RouterIdentity m_theirIdentity;
				Endpoint m_theirEndpoint;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
