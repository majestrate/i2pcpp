#include "Database.h"

#include "util/Base64.h"

namespace i2pcpp {
	Database::Database(std::string const &file)
	{
		if(sqlite3_open(file.c_str(), &m_db) != SQLITE_OK) {} // TODO Exception

		sqlite3_exec(m_db, "PRAGMA foreign_keys=ON", NULL, NULL, NULL);
		sqlite3_create_function(m_db, "sha256", 1, SQLITE_ANY, NULL, &sha256_func, NULL, NULL);
	}

	std::string Database::getConfigValue(std::string const &name)
	{
		const std::string select = "SELECT value FROM config WHERE name = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) {} // TODO Exception

		sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			const char *bytes = (char *)sqlite3_column_text(statement, 0);
			const int size = sqlite3_column_bytes(statement, 0);
			std::string ret(bytes, size);
			sqlite3_finalize(statement);
			return ret;
		} else {} // TODO Exception
	}

	bool Database::routerExists(RouterHash const &routerHash)
	{
		const std::string select = "SELECT COUNT(id) AS count FROM routers WHERE id = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) {} // TODO Exception

		sqlite3_bind_blob(statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW)
			return (sqlite3_column_int(statement, 0) != 0);
	}

	RouterInfo Database::getRouterInfo(std::string const &routerHash)
	{
		return getRouterInfo(Base64::decode(routerHash));
	}

	RouterInfo Database::getRouterInfo(RouterHash const &routerHash)
	{
		std::string select = "SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?";
		sqlite3_stmt *statement, *options_statement;

		int rc;
		if((rc = sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL)) != SQLITE_OK) {} // TODO Exception

		sqlite3_bind_blob(statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);

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
		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) {} // TODO Exception

		sqlite3_bind_blob(statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);

		while(sqlite3_step(statement) == SQLITE_ROW) {
			char *bytes;
			int size;

			bytes = (char *)sqlite3_column_text(statement, 0);
			size = sqlite3_column_bytes(statement, 0);
			std::string name(bytes, size);

			bytes = (char *)sqlite3_column_text(statement, 1);
			size = sqlite3_column_bytes(statement, 1);
			std::string value(bytes, size);

			router_options.setValue(name, value);
		}

		sqlite3_finalize(statement);
		
		auto pubItr = published.cbegin();
		auto certItr = certificate.cbegin();
		RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr)), Date(pubItr), router_options, signature);

		select = "SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?";
		if(sqlite3_prepare(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) {} // TODO Exception

		sqlite3_bind_blob(statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);

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
			std::string transport(bytes, size);

			select = "SELECT name, value FROM router_address_options WHERE router_id = ? AND \"index\" = ? ORDER BY name ASC";
			if(sqlite3_prepare(m_db, select.c_str(), -1, &options_statement, NULL) != SQLITE_OK) {} // TODO Exception

			sqlite3_bind_blob(options_statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);
			sqlite3_bind_int(options_statement, 2, index);

			Mapping address_options;

			while(sqlite3_step(options_statement) == SQLITE_ROW) {
				char *obytes;
				int osize;

				obytes = (char *)sqlite3_column_text(options_statement, 0);
				osize = sqlite3_column_bytes(options_statement, 0);
				std::string name(obytes, osize);

				obytes = (char *)sqlite3_column_text(options_statement, 1);
				osize = sqlite3_column_bytes(options_statement, 1);
				std::string value(obytes, osize);

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

	void Database::deleteRouter(RouterHash const &rh)
	{
		sqlite3_stmt *statement;
		int rc;

		std::string del = "DELETE FROM router_address_options WHERE router_id = ?";
		if((rc = sqlite3_prepare(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) { std::cerr << "RC: " << rc << "\n"; } // TODO Exception
		sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
		if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Delete RC: " << rc << "\n"; } // TODO Exception`
		sqlite3_finalize(statement);

		del = "DELETE FROM router_addresses WHERE router_id = ?";
		if((rc = sqlite3_prepare(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) { std::cerr << "RC: " << rc << "\n"; } // TODO Exception
		sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
		if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Delete RC: " << rc << "\n"; } // TODO Exception`
		sqlite3_finalize(statement);

		del = "DELETE FROM router_options WHERE router_id = ?";
		if((rc = sqlite3_prepare(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) { std::cerr << "RC: " << rc << "\n"; } // TODO Exception
		sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
		if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Delete RC: " << rc << "\n"; } // TODO Exception`
		sqlite3_finalize(statement);

		del = "DELETE FROM routers WHERE id = ?";
		if((rc = sqlite3_prepare(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) { std::cerr << "RC: " << rc << "\n"; } // TODO Exception
		sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
		if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Delete RC: " << rc << "\n"; } // TODO Exception`
		sqlite3_finalize(statement);
	}

	void Database::setRouterInfo(RouterInfo const &info)
	{
		sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, NULL);
		RouterHash rh = info.getIdentity().getHash();
		deleteRouter(rh);

		sqlite3_stmt *statement;
		int rc;

		const ByteArray& encKey = info.getIdentity().getEncryptionKey();
		const ByteArray& sigKey = info.getIdentity().getSigningKey();
		const ByteArray& cert   = info.getIdentity().getCertificate().getBytes();
		const ByteArray& pub    = info.getPublished().getBytes();
		const ByteArray& sig    = info.getSignature();

		std::string insert = "INSERT INTO routers(id, encryption_key, signing_key, certificate, published, signature) VALUES(?, ?, ?, ?, ?, ?)";
		if((rc = sqlite3_prepare(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) { std::cerr << "RC: " << rc << "\n"; } // TODO Exception
		sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
		sqlite3_bind_blob(statement, 2, encKey.data(), encKey.size(), SQLITE_STATIC);
		sqlite3_bind_blob(statement, 3, sigKey.data(), sigKey.size(), SQLITE_STATIC);
		sqlite3_bind_blob(statement, 4, cert.data(), cert.size(), SQLITE_STATIC);
		sqlite3_bind_blob(statement, 5, pub.data(), pub.size(), SQLITE_STATIC);
		sqlite3_bind_blob(statement, 6, sig.data(), sig.size(), SQLITE_STATIC);
		if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Insert RC: " << rc << "\n"; } // TODO Exception
		sqlite3_finalize(statement);

		int i = 0;
		for(auto& a: info) {
			std::string istr = std::to_string(i);
			insert = "INSERT INTO router_addresses(router_id, \"index\", cost, expiration, transport) VALUES(?, ?, ?, ?, ?)";

			if((rc = sqlite3_prepare(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) {} // TODO Exception
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
			sqlite3_bind_text(statement, 2, istr.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(statement, 3, (int)a.getCost());
			ByteArray expBytes = a.getExpiration().getBytes();
			sqlite3_bind_blob(statement, 4, expBytes.data(), expBytes.size(), SQLITE_STATIC);
			sqlite3_bind_text(statement, 5, a.getTransport().c_str(), -1, SQLITE_STATIC);
			if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Insert RC: " << rc << "\n"; } // TODO Exception
			sqlite3_finalize(statement);

			for(auto& o: a.getOptions()) {
				insert = "INSERT INTO router_address_options(router_id, \"index\", name, value) VALUES(?, ?, ?, ?)";
				if((rc = sqlite3_prepare(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) {} // TODO Exception
				sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
				sqlite3_bind_text(statement, 2, istr.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(statement, 3, o.first.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(statement, 4, o.second.c_str(), -1, SQLITE_STATIC);
				if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Insert[" << i << "] RC: " << rc << "\n"; } // TODO Exception
				sqlite3_finalize(statement);
			}

			i++;
		}

		for(auto& o: info.getOptions()) {
			insert = "INSERT INTO router_options(router_id, name, value) VALUES(?, ?, ?)";
			if((rc = sqlite3_prepare(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) {} // TODO Exception
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
			sqlite3_bind_text(statement, 2, o.first.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(statement, 3, o.second.c_str(), -1, SQLITE_STATIC);
			if((rc = sqlite3_step(statement)) != SQLITE_DONE) { std::cerr << "Insert[" << i << "] RC: " << rc << "\n"; } // TODO Exception
			sqlite3_finalize(statement);
		}

		sqlite3_exec(m_db, "COMMIT TRANSACTION", NULL, NULL, NULL);
	}

	void Database::sha256_func(sqlite3_context *context, int argc, sqlite3_value **argv)
	{
		if(argc != 1) {
			sqlite3_result_null(context);
			return;
		}

		int size = sqlite3_value_bytes(argv[0]);
		const unsigned char *input = (unsigned char *)sqlite3_value_blob(argv[0]);
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(input, size);

		hashPipe.end_msg();

		std::array<unsigned char, 32> output;
		hashPipe.read(output.data(), 32);

		sqlite3_result_blob(context, output.data(), 32, NULL);
	}
}
