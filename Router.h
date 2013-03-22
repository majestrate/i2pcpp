#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "transport/Transport.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile);

			~Router();

			void start();
			void stop();

		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			TransportPtr m_transport;
	};
}

#endif
