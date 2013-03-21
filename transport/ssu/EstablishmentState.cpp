#include "EstablishmentState.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
	namespace SSU {
		EstablishmentState::EstablishmentState(Botan::DL_Group const &group, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk) :
			m_group(group),
			m_dsaKey(dsaKey),
			m_direction(EstablishmentState::INBOUND),
			m_theirEndpoint(ep)
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group dh_group("modp/ietf/2048");

			m_dhKey = new Botan::DH_PrivateKey(rng, group);
		}

		EstablishmentState::EstablishmentState(Botan::DL_Group const &group, Botan::DSA_PrivateKey const &dsaKey, Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri) :
			m_group(group),
			m_dsaKey(dsaKey),
			m_direction(EstablishmentState::OUTBOUND),
			m_theirIdentity(ri),
			m_theirEndpoint(ep)
		{
			EstablishmentState(group, dsaKey, ep, sk);
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

		EstablishmentState::State EstablishmentState::getState() const
		{
			return m_state;
		}

		void EstablishmentState::setState(EstablishmentState::State state)
		{
			m_state = state;
		}

		Botan::InitializationVector EstablishmentState::getIV() const
		{
			return m_iv;
		}
	}
}
