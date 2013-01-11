#ifndef SSUMESSAGESENDER_H
#define SSUMESSAGESENDER_H

#include <boost/asio.hpp>

#include "OutboundMessageState.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class MessageSender : public boost::asio::io_service::service {
			public:
				MessageSender(UDPTransport &transport);

				void shutdown_service() {}

				void addMessage(PeerStatePtr const &ps, OutboundMessageStatePtr const &oms);

			private:
				void messageSent(PeerStatePtr &ps, OutboundMessageStatePtr &oms);

				UDPTransport& m_transport;
		};
	}
}

#endif
