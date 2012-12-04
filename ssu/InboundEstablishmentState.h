#ifndef SSUINBOUNDESTABLISHMENTSTATE_H
#define SSUINBOUNDESTABLISHMENTSTATE_H

#include <memory>

#include "../datatypes/SessionKey.h"

using namespace std;

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

		typedef shared_ptr<InboundEstablishmentState> InboundEstablishmentStatePtr;
	}
}

#endif
