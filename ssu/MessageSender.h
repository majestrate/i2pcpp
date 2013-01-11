#ifndef SSUMESSAGESENDER_H
#define SSUMESSAGESENDER_H

#include "OutboundMessageState.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class MessageSender {
			public:
				MessageSender(UDPTransport &transport);

				void addMessage(PeerStatePtr const &ps, OutboundMessageStatePtr const &oms);

			private:
				void messageSent(PeerStatePtr &ps, OutboundMessageStatePtr &oms);

				UDPTransport& m_transport;
		};
	}
}

#endif
