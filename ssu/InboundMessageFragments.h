#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class InboundMessageFragments {
			public:
				void receiveData(ByteArray::const_iterator &dataItr, PeerStatePtr const &ps);

			private:
				// TODO: Decaying bloom filter
		};
	}
}

#endif
