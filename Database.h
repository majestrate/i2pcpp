#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include <sqlite3.h>

#include "datatypes/ByteArray.h"
#include "datatypes/RouterInfo.h"

namespace i2pcpp {
	class Database {
		public:
			Database(std::string const &file);

			std::string getConfigValue(std::string const &name);
			ByteArray getConfigBlob(std::string const &name);
			RouterInfo getRouterInfo(std::string const &routerHash);

		private:
			sqlite3 *m_db;

			static void sha256_func(sqlite3_context *context, int argc, sqlite3_value **argv);
	};
}

#endif
