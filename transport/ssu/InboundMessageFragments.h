#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"

#include "../../datatypes/ByteArray.h"

#include "../../Log.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class InboundMessageFragments {
			public:
				InboundMessageFragments(UDPTransport &transport);

				void receiveData(PeerStatePtr const &ps, ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				UDPTransport &m_transport;

				i2p_logger_mt m_log;
				// TODO Decaying bloom filter
		};
	}
}

#endif
