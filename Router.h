#ifndef ROUTER_H
#define ROUTER_H

#include <boost/asio.hpp>

#include <string>
#include <thread>

#include "datatypes/ByteArray.h"

#include "Log.h"
#include "RouterContext.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile);
			Router(const Router &) = delete;
			Router& operator=(Router &) = delete;
			~Router();

			void start();
			void stop();

			ByteArray getRouterInfo();
			void importRouter(RouterInfo const &router);
			void importRouter(std::vector<RouterInfo> const &routers);
			void deleteAllRouters();
			void setConfigValue(std::string key, std::string value);
			std::string getConfigValue(std::string key);

		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			RouterContext m_ctx;

			i2p_logger_mt m_log;
	};
}

#endif
