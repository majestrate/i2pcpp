#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <mutex>
#include <map>

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"
#include "handlers/Message.h"

namespace i2pcpp {
	class InboundMessageDispatcher : public boost::asio::io_service::service {
		public:
			InboundMessageDispatcher(boost::asio::io_service &ios) : boost::asio::io_service::service::service(ios) {}

			void shutdown_service() {}

			void receiveMessage(RouterHash const &from, I2NP::MessagePtr const &msg);
			void registerHandler(I2NP::Message::Type const mtype, Handlers::MessagePtr const &handler);
			void unregisterHandler(I2NP::Message::Type const mtype);

		private:
			std::map<I2NP::Message::Type, Handlers::MessagePtr> m_msgHandlers;

			std::mutex m_mutex;
	};
}

#endif
