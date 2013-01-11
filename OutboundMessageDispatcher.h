#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

#include "Transport.h"

namespace i2pcpp {
	class OutboundMessageDispatcher : public boost::asio::io_service::service {
		public:
			OutboundMessageDispatcher(boost::asio::io_service &ios) : boost::asio::io_service::service::service(ios) {}

			void shutdown_service() {}

			void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg);
			void registerTransport(TransportPtr const &t);

		private:
			TransportPtr m_transport;
	};
}

#endif
