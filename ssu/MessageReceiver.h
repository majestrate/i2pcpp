#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

#include <boost/asio.hpp>

#include "InboundMessageState.h"

namespace i2pcpp {
	class RouterContext;

	namespace SSU {
		class UDPTransport;
		class MessageReceiver : public boost::asio::io_service::service {
			public:
				MessageReceiver(UDPTransport &transport);

				void shutdown_service() {}

				void addMessage(InboundMessageStatePtr const &ims);

			private:
				void messageReceived(InboundMessageStatePtr const &ims);

				RouterContext& m_ctx;
		};
	}
}

#endif
