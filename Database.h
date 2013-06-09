#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include <sqlite3.h>

#include "datatypes/ByteArray.h"
#include "datatypes/RouterInfo.h"
#include "Log.h"

namespace i2pcpp {
	class Database {
		public:
			Database(std::string const &file);
			~Database();

			std::string getConfigValue(std::string const &name);
			void setConfigValue(std::string const &name, std::string const &value);
			ByteArray getConfigBlob(std::string const &name);
			RouterHash getRandomFloodfill();
			bool routerExists(RouterHash const &routerHash);
			RouterInfo getRouterInfo(std::string const &routerHash);
			RouterInfo getRouterInfo(RouterHash const &routerHash);
			void deleteRouter(RouterHash const &hash);
			void deleteAllRouters();
			void setRouterInfo(std::vector<RouterInfo> const &routers);
			void setRouterInfo(RouterInfo const &info, bool transaction = true);

		private:
			i2p_logger_mt  m_log;
			sqlite3 *m_db;

			mutable std::mutex m_mutex;

			static void sha256_func(sqlite3_context *context, int argc, sqlite3_value **argv);
	};
}

#endif
