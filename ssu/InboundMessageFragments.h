#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class InboundMessageFragments {
			public:
				InboundMessageFragments(UDPTransport &transport);

				void receiveData(PeerStatePtr const &ps, ByteArray::const_iterator &dataItr);

			private:
				UDPTransport &m_transport;
				// TODO Decaying bloom filter
		};
	}
}

#endif
