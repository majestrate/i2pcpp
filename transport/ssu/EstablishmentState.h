#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include <botan/dh.h>
#include <botan/dsa.h>
#include <botan/botan.h>

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"
#include "../../datatypes/RouterIdentity.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(Botan::DL_Group const &group, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk);
				EstablishmentState(Botan::DL_Group const &group, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri);
				EstablishmentState(EstablishmentState const &state) = delete;
				~EstablishmentState();

				enum Direction {
					INBOUND,
					OUTBOUND
				};

				enum State {
					UNKNOWN,
					REQUEST_SENT,
					REQUEST_RECEIVED,
					CREATED_SENT,
					CREATED_RECEIVED,
					CONFIRMED_SENT,
					CONFIRMED_RECEIVED
				};

				Direction getDirection() const;
				State getState() const;
				void setState(State state);
				Botan::InitializationVector getIV() const;

			private:
				const Botan::DL_Group& m_group;
				const Botan::DSA_PrivateKey& m_dsaKey;

				Direction m_direction;
				Botan::DH_PrivateKey *m_dhKey;

				RouterIdentity m_theirIdentity;
				Endpoint m_theirEndpoint;
				ByteArray m_theirDH;

				Botan::InitializationVector m_iv;
				ByteArray m_dhSecret;

				State m_state = UNKNOWN;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
