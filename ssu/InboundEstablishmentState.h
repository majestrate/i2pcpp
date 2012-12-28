#ifndef SSUINBOUNDESTABLISHMENTSTATE_H
#define SSUINBOUNDESTABLISHMENTSTATE_H

#include <memory>

#include "../datatypes/SessionKey.h"

namespace i2pcpp {
	namespace SSU {
		class InboundEstablishmentState {
			public:
				enum State {
					UNKNOWN,
					REQUEST_RECEIVED,
					CREATED_SENT,
					CONFIRMED_PARTIALLY,
					COMPLETELY,
					FAILED,
					COMPLETE
				};

			private:
		};

		typedef std::shared_ptr<InboundEstablishmentState> InboundEstablishmentStatePtr;
	}
}

#endif
