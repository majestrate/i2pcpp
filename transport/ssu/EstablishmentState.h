#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"
#include "../../datatypes/RouterIdentity.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(Endpoint const &ep, SessionKey const &sessionKey);
				EstablishmentState(Endpoint const &ep, SessionKey const &sessionKey, RouterIdentity const &ri);

				enum Direction {
					INBOUND,
					OUTBOUND
				};

				Direction getDirection() const;

			private:
				Direction m_direction;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
