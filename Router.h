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

			~Router();

			void start();
			void stop();

			void importNetDb(std::string const &dirname);
			ByteArray getRouterInfo();
			void importRouterInfo(ByteArray const &info);
			void setConfigValue(std::string key, std::string value);
			std::string getConfigValue(std::string key);

			i2p_logger_mt & getLogger();
		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			RouterContext m_ctx;

			i2p_logger_mt m_log;
	};
}

#endif
