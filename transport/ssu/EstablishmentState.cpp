#include "EstablishmentState.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
	namespace SSU {
		EstablishmentState::EstablishmentState(Botan::DL_Group const &params, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk) :
			m_direction(EstablishmentState::INBOUND),
			m_theirEndpoint(ep)
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group group("modp/ietf/2048");

			m_dhKey = new Botan::DH_PrivateKey(rng, group);
		}

		EstablishmentState::EstablishmentState(Botan::DL_Group const &params, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri) :
			m_direction(EstablishmentState::OUTBOUND),
			m_theirIdentity(ri),
			m_theirEndpoint(ep)
		{
			EstablishmentState(params, dsaKey, ep, sk);
		}

		EstablishmentState::~EstablishmentState()
		{
			if(m_dhKey)
				delete m_dhKey;
		}

		EstablishmentState::Direction EstablishmentState::getDirection() const
		{
			return m_direction;
		}
	}
}
