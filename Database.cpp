#include "Database.h"

#include <sys/stat.h>

#include <botan/auto_rng.h>
#include <botan/elgamal.h>
#include <botan/pkcs8.h>
#include <botan/dsa.h>

#include <boost/tokenizer.hpp>

#include "exceptions/StatementPrepareError.h"
#include "exceptions/RecordNotFound.h"
#include "exceptions/SQLError.h"

#include "util/Base64.h"
#include "util/I2PDH.h"

extern uint8_t _binary_schema_sql_start[];
extern uintptr_t _binary_schema_sql_size[];

namespace i2pcpp {
	Database::Database(std::string const &file)
	{
		struct stat buffer;
		if(stat(file.c_str(), &buffer) != 0)
			throw std::runtime_error("database file " + file + " does not exist");

		if(sqlite3_open(file.c_str(), &m_db) != SQLITE_OK)
			throw std::runtime_error("could not open database");

		sqlite3_exec(m_db, "PRAGMA foreign_keys=ON", NULL, NULL, NULL);
	}

	Database::~Database()
	{
		sqlite3_close(m_db);
	}

	void Database::createDb(std::string const &file)
	{
		sqlite3 *db;

		if(sqlite3_open(file.c_str(), &db) != SQLITE_OK)
			throw std::runtime_error("could not create database");

		sqlite3_exec(db, "PRAGMA foreign_keys=ON", NULL, NULL, NULL);

		sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

		std::string schema((char *)_binary_schema_sql_start, (uintptr_t)_binary_schema_sql_size);

		boost::char_separator<char> sep(";");
		boost::tokenizer<boost::char_separator<char>> tok(schema, sep);

		auto itr = tok.begin();
		for(auto s: tok) {
			if(s == "\n" || s.size() < 2) continue;

			sqlite3_exec(db, s.c_str(), NULL, NULL, NULL);
		}

		Botan::AutoSeeded_RNG rng;
		Botan::DSA_PrivateKey dsa_key(rng, DH::getGroup());
		Botan::ElGamal_PrivateKey elg_key(rng, Botan::DL_Group("modp/ietf/2048"));
		std::string elg_string = Botan::PKCS8::PEM_encode(elg_key);
		std::string dsa_string = Botan::PKCS8::PEM_encode(dsa_key);

		sqlite3_stmt *statement;

		std::string insert = "INSERT OR REPLACE INTO config (name, value) VALUES ('private_encryption_key', ?)";
		if(sqlite3_prepare_v2(db, insert.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		sqlite3_bind_text(statement, 1, elg_string.c_str(), -1, SQLITE_STATIC);

		if(sqlite3_step(statement) != SQLITE_DONE) {
			sqlite3_finalize(statement);
			throw SQLError(insert);
		}

		sqlite3_finalize(statement);

		insert = "INSERT OR REPLACE INTO config (name, value) VALUES ('private_signing_key', ?)";
		if(sqlite3_prepare_v2(db, insert.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		sqlite3_bind_text(statement, 1, dsa_string.c_str(), -1, SQLITE_STATIC);

		if(sqlite3_step(statement) != SQLITE_DONE) {
			sqlite3_finalize(statement);
			throw SQLError(insert);
		}

		sqlite3_finalize(statement);

		sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);
		sqlite3_close(db);
	}

	std::string Database::getConfigValue(std::string const &name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		const std::string select = "SELECT value FROM config WHERE name = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			const char *bytes = (char *)sqlite3_column_text(statement, 0);
			const int size = sqlite3_column_bytes(statement, 0);
			std::string ret(bytes, size);

			sqlite3_finalize(statement);
			return ret;
		} else {
			sqlite3_finalize(statement);
			throw RecordNotFound(name);
		}
	}

	void Database::setConfigValue(std::string const &name, std::string const &value)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		const std::string insert = "INSERT OR REPLACE INTO config (name, value) VALUES (?, ?)";
		sqlite3_stmt *statement;

		if(sqlite3_prepare_v2(m_db, insert.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(statement, 2, value.c_str(), -1, SQLITE_STATIC);

		if(sqlite3_step(statement) != SQLITE_DONE) {
			sqlite3_finalize(statement);
			throw SQLError(insert);
		}

		sqlite3_finalize(statement);
	}

	RouterHash Database::getRandomRouter()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		//const std::string select = "SELECT router_id FROM router_options WHERE router_options.name='caps' AND router_options.value LIKE '%f%' ORDER BY RANDOM() LIMIT 1";
		const std::string select = "SELECT router_id FROM routers ORDER BY RANDOM() LIMIT 1";
		sqlite3_stmt *statement;

		if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			unsigned char *bytes = (unsigned char *)sqlite3_column_blob(statement, 0);
			int size = sqlite3_column_bytes(statement, 0);
			RouterHash rh;
			std::copy(bytes, bytes + size, rh.begin());

			sqlite3_finalize(statement);
			return rh;
		} else {
			sqlite3_finalize(statement);
			throw RecordNotFound("random router");
		}
	}

	bool Database::routerExists(RouterHash const &routerHash)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		const std::string select = "SELECT COUNT(id) AS count FROM routers WHERE id = ?";
		sqlite3_stmt *statement;

		if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		sqlite3_bind_blob(statement, 1, routerHash.data(), routerHash.size(), SQLITE_STATIC);

		int rc = sqlite3_step(statement);
		if(rc == SQLITE_ROW) {
			bool exists = (sqlite3_column_int(statement, 0) != 0);

			sqlite3_finalize(statement);
			return exists;
		}

		sqlite3_finalize(statement);
		throw std::runtime_error("Definitely should have gotten a row back here");
	}

	RouterInfo Database::getRouterInfo(std::string const &routerHash)
	{
		return getRouterInfo(Base64::decode(routerHash));
	}

	RouterInfo Database::getRouterInfo(RouterHash const &routerHash)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		sqlite3_stmt *statement, *options_statement;

		try {
			std::string select = "SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?";

			int rc;
			if((rc = sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

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
			} else
				throw RecordNotFound(std::string(routerHash));

			Mapping router_options;

			select = "SELECT name, value FROM router_options WHERE router_id = ?";
			if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

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
			auto pubEndItr = published.cend();
			auto certItr = certificate.cbegin();
			auto certEndItr = certificate.cend();
			RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr, certEndItr)), Date(pubItr, pubEndItr), router_options, signature);

			select = "SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?";
			if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

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
				if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &options_statement, NULL) != SQLITE_OK) throw StatementPrepareError();

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
				auto expEndItr = expiration.cend();
				RouterAddress ra(cost, Date(expItr, expEndItr), transport, address_options);
				ri.addAddress(ra);
			}

			sqlite3_finalize(statement);

			return ri;
		} catch(std::exception &e) {
			sqlite3_finalize(statement);
			throw;
		}
	}

	void Database::deleteRouter(RouterHash const &rh)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		sqlite3_stmt *statement;
		int rc;

		try {
			sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, NULL);

			std::string del = "DELETE FROM profiles WHERE router_id = ?";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_address_options WHERE router_id = ?";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_addresses WHERE router_id = ?";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_options WHERE router_id = ?";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM routers WHERE id = ?";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			sqlite3_exec(m_db, "COMMIT TRANSACTION", NULL, NULL, NULL);
		} catch(SQLError &e) {
			sqlite3_finalize(statement);
			throw;
		}
	}

	void Database::deleteAllRouters()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		sqlite3_stmt *statement;
		int rc;

		try {
			sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, NULL);

			std::string del = "DELETE FROM profiles";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_address_options";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_addresses";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM router_options";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			del = "DELETE FROM routers";
			if((rc = sqlite3_prepare_v2(m_db, del.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(del);

			sqlite3_finalize(statement);

			sqlite3_exec(m_db, "COMMIT TRANSACTION", NULL, NULL, NULL);
		} catch(SQLError &e) {
			sqlite3_finalize(statement);
		}
	}

	void Database::setRouterInfo(std::vector<RouterInfo> const &routers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, NULL);

		for(auto r: routers)
			setRouterInfo(r, false);

		sqlite3_exec(m_db, "COMMIT TRANSACTION", NULL, NULL, NULL);
	}

	void Database::setRouterInfo(RouterInfo const &info, bool transaction)
	{
		sqlite3_stmt *statement;

		try {
			if(transaction) {
				m_mutex.lock();
				sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, NULL);
			}

			RouterHash rh = info.getIdentity().getHash();

			int rc;

			const ByteArray& encKey = info.getIdentity().getEncryptionKey();
			const ByteArray& sigKey = info.getIdentity().getSigningKey();
			const ByteArray& cert   = info.getIdentity().getCertificate().serialize();
			const ByteArray& pub    = info.getPublished().serialize();
			const ByteArray& sig    = info.getSignature();

			std::string insert = "INSERT OR REPLACE INTO routers(id, encryption_key, signing_key, certificate, published, signature) VALUES(?, ?, ?, ?, ?, ?)";
			if((rc = sqlite3_prepare_v2(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
			sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
			sqlite3_bind_blob(statement, 2, encKey.data(), encKey.size(), SQLITE_STATIC);
			sqlite3_bind_blob(statement, 3, sigKey.data(), sigKey.size(), SQLITE_STATIC);
			sqlite3_bind_blob(statement, 4, cert.data(), cert.size(), SQLITE_STATIC);
			sqlite3_bind_blob(statement, 5, pub.data(), pub.size(), SQLITE_STATIC);
			sqlite3_bind_blob(statement, 6, sig.data(), sig.size(), SQLITE_STATIC);

			if((rc = sqlite3_step(statement)) != SQLITE_DONE)
				throw SQLError(insert);

			sqlite3_finalize(statement);

			int i = 0;
			for(auto& a: info) {
				std::string istr = std::to_string(i);
				insert = "INSERT OR REPLACE INTO router_addresses(router_id, \"index\", cost, expiration, transport) VALUES(?, ?, ?, ?, ?)";

				if((rc = sqlite3_prepare_v2(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
				sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
				sqlite3_bind_text(statement, 2, istr.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_int(statement, 3, (int)a.getCost());
				ByteArray expBytes = a.getExpiration().serialize();
				sqlite3_bind_blob(statement, 4, expBytes.data(), expBytes.size(), SQLITE_STATIC);
				sqlite3_bind_text(statement, 5, a.getTransport().c_str(), -1, SQLITE_STATIC);

				if((rc = sqlite3_step(statement)) != SQLITE_DONE)
					throw SQLError(insert);

				sqlite3_finalize(statement);

				for(auto& o: a.getOptions()) {
					insert = "INSERT OR REPLACE INTO router_address_options(router_id, \"index\", name, value) VALUES(?, ?, ?, ?)";
					if((rc = sqlite3_prepare_v2(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
					sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
					sqlite3_bind_text(statement, 2, istr.c_str(), -1, SQLITE_STATIC);
					sqlite3_bind_text(statement, 3, o.first.c_str(), -1, SQLITE_STATIC);
					sqlite3_bind_text(statement, 4, o.second.c_str(), -1, SQLITE_STATIC);

					if((rc = sqlite3_step(statement)) != SQLITE_DONE)
						throw SQLError(insert);

					sqlite3_finalize(statement);
				}

				i++;
			}

			for(auto& o: info.getOptions()) {
				insert = "INSERT OR REPLACE INTO router_options(router_id, name, value) VALUES(?, ?, ?)";
				if((rc = sqlite3_prepare_v2(m_db, insert.c_str(), -1, &statement, NULL)) != SQLITE_OK) throw StatementPrepareError();
				sqlite3_bind_blob(statement, 1, rh.data(), rh.size(), SQLITE_STATIC);
				sqlite3_bind_text(statement, 2, o.first.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(statement, 3, o.second.c_str(), -1, SQLITE_STATIC);

				if((rc = sqlite3_step(statement)) != SQLITE_DONE)
					throw SQLError(insert);

				sqlite3_finalize(statement);
			}

			if(transaction) {
				sqlite3_exec(m_db, "COMMIT TRANSACTION", NULL, NULL, NULL);
				m_mutex.unlock();
			}
		} catch(SQLError &e) {
			sqlite3_finalize(statement);
			m_mutex.unlock();
			throw;
		}
	}

	std::forward_list<RouterHash> Database::getAllHashes()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::forward_list<RouterHash> hashes;

		const std::string select = "SELECT id FROM routers";
		sqlite3_stmt *statement;

		if(sqlite3_prepare_v2(m_db, select.c_str(), -1, &statement, NULL) != SQLITE_OK) throw StatementPrepareError();

		while(sqlite3_step(statement) == SQLITE_ROW) {
			unsigned char *bytes = (unsigned char *)sqlite3_column_blob(statement, 0);
			int size = sqlite3_column_bytes(statement, 0);
			RouterHash rh;
			std::copy(bytes, bytes + size, rh.begin());
			hashes.push_front(rh);
		}

		sqlite3_finalize(statement);

		return hashes;
	}
}
