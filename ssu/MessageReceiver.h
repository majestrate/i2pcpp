#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class MessageReceiver {
			public:
				MessageReceiver(UDPTransport &transport);

				void addMessage(InboundMessageStatePtr const &ims);

			private:
				void messageReceived(InboundMessageStatePtr const &ims);

				UDPTransport &m_transport;
		};
	}
}

#endif
