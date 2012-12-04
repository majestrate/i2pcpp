#include "Database.h"

#include <array>
#include <exception>
#include <iostream>

#include <botan/pipe.h>
#include <botan/lookup.h>

#include "util/Base64.h"

using namespace std;
using namespace Botan;

namespace i2pcpp {
	Database::Database(string const &file)
	{
		if(sqlite3_open(file.c_str(), &m_db) != SQLITE_OK)
			throw exception();

		sqlite3_exec(m_db, "PRAGMA foreign_keys=ON", NULL, NULL, NULL);
		sqlite3_create_function(m_db, "sha256", 1, SQLITE_ANY, NULL, &sha256_func, NULL, NULL);
	}

	string Database::getConfigValue(string const &name)
	{
		const string select = "SELECT value FROM config WHERE name = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK)
			throw exception();

		sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			const char *bytes = (char *)sqlite3_column_text(statement, 0);
			const int size = sqlite3_column_bytes(statement, 0);
			string ret(bytes, size);
			sqlite3_finalize(statement);
			return ret;
		} else
			throw exception();
	}

	ByteArray Database::getConfigBlob(string const &name)
	{
		const string select = "SELECT data FROM config_blobs WHERE name = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK)
			throw exception();

		sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			const unsigned char *bytes = (unsigned char *)sqlite3_column_blob(statement, 0);
			const int size = sqlite3_column_bytes(statement, 0);
			ByteArray ret(256, 0);
			copy(bytes, bytes + size, ret.begin());
			sqlite3_finalize(statement);

			return ret;
		} else
			throw exception();
	}

	RouterInfo Database::getRouterInfo(string const &routerHash)
	{
		string select = "SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?";
		sqlite3_stmt *statement, *options_statement;

		int rc;
		if((rc = sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL)) != SQLITE_OK)
			throw exception();

		ByteArray hashBytes = Base64::decode(routerHash);
		sqlite3_bind_blob(statement, 1, hashBytes.data(), hashBytes.size(), SQLITE_STATIC);

		ByteArray encryptionKey, signingKey, certificate, published, signature;
		rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			unsigned char *bytes;
			int size;

			bytes = (unsigned char *)sqlite3_column_blob(statement, 0);
			size = sqlite3_column_bytes(statement, 0);
			encryptionKey.resize(size);
			copy(bytes, bytes + size, encryptionKey.begin());

			bytes = (unsigned char *)sqlite3_column_blob(statement, 1);
			size = sqlite3_column_bytes(statement, 1);
			signingKey.resize(size);
			copy(bytes, bytes + size, signingKey.begin());

			bytes = (unsigned char *)sqlite3_column_blob(statement, 2);
			size = sqlite3_column_bytes(statement, 2);
			certificate.resize(size);
			copy(bytes, bytes + size, certificate.begin());

			bytes = (unsigned char *)sqlite3_column_blob(statement, 3);
			size = sqlite3_column_bytes(statement, 3);
			published.resize(size);
			copy(bytes, bytes + size, published.begin());

			bytes = (unsigned char *)sqlite3_column_blob(statement, 4);
			size = sqlite3_column_bytes(statement, 4);
			signature.resize(size);
			copy(bytes, bytes + size, signature.begin());

			sqlite3_finalize(statement);
		}

		Mapping router_options;

		select = "SELECT name, value FROM router_options WHERE router_id = ?";
		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK)
			throw exception();

		sqlite3_bind_blob(statement, 1, hashBytes.data(), hashBytes.size(), SQLITE_STATIC);

		while(sqlite3_step(statement) == SQLITE_ROW) {
			char *bytes;
			int size;

			bytes = (char *)sqlite3_column_text(statement, 0);
			size = sqlite3_column_bytes(statement, 0);
			string name(bytes, size);

			bytes = (char *)sqlite3_column_text(statement, 1);
			size = sqlite3_column_bytes(statement, 1);
			string value(bytes, size);

			router_options.setValue(name, value);
		}

		sqlite3_finalize(statement);
		
		auto pubItr = published.cbegin();
		auto certItr = certificate.cbegin();
		RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr)), Date(pubItr), router_options, signature);

		select = "SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?";
		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK)
			throw exception();

		sqlite3_bind_blob(statement, 1, hashBytes.data(), hashBytes.size(), SQLITE_STATIC);

		while(sqlite3_step(statement) == SQLITE_ROW) {
			char *bytes;
			int size;

			int index = sqlite3_column_int(statement, 0);

			int cost = sqlite3_column_int(statement, 1);

			bytes = (char *)sqlite3_column_blob(statement, 2);
			size = sqlite3_column_bytes(statement, 2);
			ByteArray expiration(size);
			copy(bytes, bytes + size, expiration.begin());

			bytes = (char *)sqlite3_column_text(statement, 3);
			size = sqlite3_column_bytes(statement, 3);
			string transport(bytes, size);

			select = "SELECT name, value FROM router_address_options WHERE router_id = ? AND \"index\" = ?";
			if(sqlite3_prepare(m_db, select.c_str(), -1, &options_statement, NULL) != SQLITE_OK)
				throw exception();

			sqlite3_bind_blob(options_statement, 1, hashBytes.data(), hashBytes.size(), SQLITE_STATIC);
			sqlite3_bind_int(options_statement, 2, index);

			Mapping address_options;

			while(sqlite3_step(options_statement) == SQLITE_ROW) {
				char *obytes;
				int osize;

				obytes = (char *)sqlite3_column_text(options_statement, 0);
				osize = sqlite3_column_bytes(options_statement, 0);
				string name(obytes, osize);

				obytes = (char *)sqlite3_column_text(options_statement, 1);
				osize = sqlite3_column_bytes(options_statement, 1);
				string value(obytes, osize);

				address_options.setValue(name, value);
			}

			sqlite3_finalize(options_statement);

			auto expItr = expiration.cbegin();
			RouterAddress ra(cost, Date(expItr), transport, address_options);
			ri.addAddress(ra);
		}

		sqlite3_finalize(statement);

		return ri;

	}

	void Database::sha256_func(sqlite3_context *context, int argc, sqlite3_value **argv)
	{
		if(argc != 1) {
			sqlite3_result_null(context);
			return;
		}

		int size = sqlite3_value_bytes(argv[0]);
		const unsigned char *input = (unsigned char *)sqlite3_value_blob(argv[0]);
		Pipe hashPipe(new Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(input, size);

		hashPipe.end_msg();

		array<unsigned char, 32> output;
		hashPipe.read(output.data(), 32);

		sqlite3_result_blob(context, output.data(), 32, NULL);
	}
}
