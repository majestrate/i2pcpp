#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"

#include "../../datatypes/ByteArray.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class InboundMessageFragments {
			public:
				InboundMessageFragments(UDPTransport &transport);

				void receiveData(PeerStatePtr const &ps, ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				UDPTransport &m_transport;
				// TODO Decaying bloom filter
		};
	}
}

#endif
