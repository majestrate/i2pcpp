#include "EstablishmentState.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentState::EstablishmentState(Endpoint const &ep, SessionKey const &sessionKey) :
			m_direction(EstablishmentState::INBOUND)
		{
		}

		EstablishmentState::EstablishmentState(Endpoint const &ep, SessionKey const &sessionKey, RouterIdentity const &ri) :
			m_direction(EstablishmentState::INBOUND)
		{
		}

		EstablishmentState::Direction EstablishmentState::getDirection() const
		{
			return m_direction;
		}
	}
}
