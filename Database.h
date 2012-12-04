#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include <sqlite3.h>

#include "datatypes/RouterInfo.h"
#include "datatypes/ByteArray.h"

using namespace std;

namespace i2pcpp {
	class Database {
		public:
			Database(string const &file);

			string getConfigValue(string const &name);
			ByteArray getConfigBlob(string const &name);
			RouterInfo getRouterInfo(string const &routerHash);

		private:
			sqlite3 *m_db;

			static void sha256_func(sqlite3_context *context, int argc, sqlite3_value **argv);
	};
}

#endif
