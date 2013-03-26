#ifndef SSUOUTBOUNDMESSAGEFRAGMENTS_H
#define SSUOUTBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class OutboundMessageFragments {
			public:
				OutboundMessageFragments(UDPTransport &transport);

				void sendData(PeerStatePtr const &ps, ByteArray const &data);
				void sendDataCallback(PeerStatePtr ps, OutboundMessageStatePtr oms);

			private:
				UDPTransport &m_transport;
		};
	}
}

#endif
