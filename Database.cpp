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

    sql_transaction Database::query(std::string const & data) 
    {
        auto itr = m_queries.find(data);
        if( itr == m_queries.end()) {
            sqlite3_stmt * stmt;
            if(sqlite3_prepare_v2(m_db, data.c_str(), -1, &stmt, NULL) != SQLITE_OK) throw SQLError(data);
            m_queries[data] = stmt;
        }
        return sql_transaction(m_queries[data]);
    }

    sql_transaction::sql_transaction(sqlite3_stmt * query) : m_stmt(query) {}

    sql_transaction::~sql_transaction() {
        sqlite3_clear_bindings(m_stmt);
        sqlite3_reset(m_stmt);
    }

    void sql_transaction::bind(int col, int data)
    {
        sqlite3_bind_int(m_stmt, col, data);
    }

    void sql_transaction::bind(int col, std::string const & data)
    {
        sqlite3_bind_text(m_stmt, col, data.c_str(), -1 , SQLITE_STATIC);
    }

    void sql_transaction::bind(int col, const void * data, uint32_t len)
    {
        sqlite3_bind_blob(m_stmt, col, data, len, SQLITE_STATIC);
    }

    int sql_transaction::get_int(int col)
    {
        return sqlite3_column_int(m_stmt, col);
    }
    
    std::string sql_transaction::get_str(int col)
    {
        const char * bytes = (char*) sqlite3_column_text(m_stmt, col);
        int size = sqlite3_column_bytes(m_stmt, col);
        return std::string(bytes, size);
    }
    
    const void * sql_transaction::get_blob(int col)
    {
        return sqlite3_column_blob(m_stmt, col);
    }

    int sql_transaction::get_bytes(int col)
    {
        return sqlite3_column_bytes(m_stmt, col);
    }

    int sql_transaction::step()
    {
        return sqlite3_step(m_stmt);
    }

    Database::Database(std::string const &file)
    {
        struct stat buffer;
        if(stat(file.c_str(), &buffer) != 0)
            throw std::runtime_error("database file " + file + " does not exist");

        if(sqlite3_open(file.c_str(), &m_db) != SQLITE_OK)
            throw std::runtime_error("could not open database");

        db_exec("PRAGMA foreign_keys=ON");
        db_exec("PRAGMA synchronous=OFF");
        db_exec("PRAGMA temp_store=MEMORY");
        
    }

    Database::~Database()
    {
        for(auto & i: m_queries) sqlite3_finalize(std::get<1>(i));
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

        const std::string sql = "SELECT value FROM config WHERE name = ?";
        auto trans = query(sql);
        trans.bind(1, name);
        if (trans.step() == SQLITE_ROW) {
            return trans.get_str(0);
        }
        throw RecordNotFound(name);
    }

    void Database::setConfigValue(std::string const &name, std::string const &value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        const std::string sql = "INSERT OR REPLACE INTO config (name, value) VALUES (?, ?)";
        auto trans = query(sql);
        trans.bind(1,name);
        trans.bind(2,value);
        if(trans.step() != SQLITE_DONE) throw SQLError(sql);
    }

    RouterHash Database::getRandomRouter()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        //const std::string select = "SELECT router_id FROM router_options WHERE router_options.name='caps' AND router_options.value LIKE '%f%' ORDER BY RANDOM() LIMIT 1";
        const std::string sql = "SELECT id FROM routers ORDER BY RANDOM() LIMIT 1";
        auto trans = query(sql);
        if (trans.step() == SQLITE_ROW) {
            unsigned char * bytes = (unsigned char*) trans.get_blob(0);
            int size = trans.get_bytes(0);
            RouterHash rh;
            std::copy(bytes, bytes + size, rh.begin());
            return rh;
        }
        throw RecordNotFound("random router");
    }

    bool Database::routerExists(RouterHash const &routerHash)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        const std::string sql = "SELECT COUNT(id) AS count FROM routers WHERE id = ?";
        auto trans = query(sql);

        trans.bind(1,routerHash.data(), routerHash.size());

        if (trans.step() == SQLITE_ROW) return trans.get_int(0) != 0;

        throw std::runtime_error("Definitely should have gotten a row back here");
    }

    RouterInfo Database::getRouterInfo(std::string const &routerHash)
    {
        return getRouterInfo(toRouterHash(Base64::decode(routerHash)));
    }

    RouterInfo Database::getRouterInfo(RouterHash const &routerHash)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        ByteArray encryptionKey, signingKey, certificate, published, signature;
        {
            const std::string sql = "SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?";
            auto trans = query(sql);
            trans.bind(1, routerHash.data(), routerHash.size());
            
            if(trans.step() == SQLITE_ROW) {
                unsigned char * bytes;
                int size;
                
                bytes = (unsigned char*) trans.get_blob(0);
                size = trans.get_bytes(0);
                encryptionKey.resize(size);
                copy(bytes, bytes + size, encryptionKey.begin());
                
                bytes = (unsigned char*) trans.get_blob(1);
                size = trans.get_bytes(1);
                signingKey.resize(size);
                copy(bytes, bytes + size, signingKey.begin());
                
                bytes = (unsigned char*) trans.get_blob(2);
                size = trans.get_bytes(2);
                certificate.resize(size);
                copy(bytes, bytes + size, certificate.begin());
                
                bytes = (unsigned char*) trans.get_blob(3);
                size = trans.get_bytes(3);
                published.resize(size);
                copy(bytes, bytes + size, published.begin());
                
                bytes = (unsigned char*) trans.get_blob(4);
                size = trans.get_bytes(4);
                signature.resize(size);
                copy(bytes, bytes + size, signature.begin());
            } else 
                throw RecordNotFound(Base64::encode(routerHash));
        }

        Mapping router_options;
        {
            const std::string sql = "SELECT name, value FROM router_options WHERE router_id = ?";
            auto trans = query(sql);
            trans.bind(1,routerHash.data(), routerHash.size());
            while (trans.step() == SQLITE_ROW) {
                auto name = trans.get_str(0);
                auto value = trans.get_str(1);
                router_options.setValue(name, value);
            }
        }
        
        auto pubItr = published.cbegin();
        auto pubEndItr = published.cend();
        auto certItr = certificate.cbegin();
        auto certEndItr = certificate.cend();
        RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr, certEndItr)), Date(pubItr, pubEndItr), router_options, signature);

        {
            const std::string sql1 = "SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?";
            const std::string sql2 = "SELECT name, value FROM router_address_options WHERE router_id = ? AND \"index\" = ? ORDER BY name ASC";
            auto trans1 = query(sql1);
            trans1.bind(1,routerHash.data(), routerHash.size());
            while( trans1.step() == SQLITE_ROW ) {
                Mapping address_options;
                char * bytes;
                int size;
                
                int index = trans1.get_int(0);
                int cost = trans1.get_int(1);
                
                bytes =  (char *) trans1.get_blob(2);
                size = trans1.get_bytes(2);
                ByteArray expiration(size);
                copy(bytes, bytes + size, expiration.begin());

                auto transport = trans1.get_str(3);

                auto trans2 = query(sql2);
                trans2.bind(1,routerHash.data(), routerHash.size());
                trans2.bind(2,index);
                
                while( trans2.step() == SQLITE_ROW ) {
                    auto name = trans2.get_str(0);
                    auto value = trans2.get_str(1);
                    address_options.setValue(name, value);
                }

                auto expItr = expiration.cbegin();
                auto expEndItr = expiration.cend();
                RouterAddress ra(cost, Date(expItr, expEndItr), transport, address_options);
                ri.addAddress(ra);
            }
        }
        return ri;
    }

    void Database::deleteRouter(RouterHash const &rh)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::vector<std::string> queries = {
            "DELETE FROM profiles WHERE router_id = ?",
            "DELETE FROM router_address_options WHERE router_id = ?",
            "DELETE FROM router_addresses WHERE router_id = ?",
            "DELETE FROM router_options WHERE router_id = ?",
            "DELETE FROM routers WHERE id = ?"            
        };

        db_exec("BEGIN TRANSACTION");

        for (auto sql :queries) {
            auto trans = query(sql);
            trans.bind(1, rh.data(), rh.size());
            if(trans.step() != SQLITE_DONE) throw SQLError(sql);
        }

        db_exec("COMMIT TRANSACTION");
    }

    void Database::deleteAllRouters()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::vector<std::string> queries = {
            "DELETE FROM profiles",
            "DELETE FROM router_address_options",
            "DELETE FROM router_addresses",
            "DELETE FROM router_options",
            "DELETE FORM routers"
        };

        db_exec("BEGIN TRANSACTION");
        
        for( auto sql : queries) {
            auto trans = query(sql);
            if(trans.step() != SQLITE_DONE) throw SQLError(sql);
        }

        db_exec("COMMIT TRANSACTION");
    }

    void Database::setRouterInfo(std::vector<RouterInfo> const &routers)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        db_exec("BEGIN TRANSACTION");

        for(auto& r: routers)
            setRouterInfo(r, false);

        db_exec("COMMIT TRANSACTION");
    }

    void Database::setRouterInfo(RouterInfo const &info, bool transaction)
    {
        if(transaction) {
            m_mutex.lock();
            db_exec("BEGIN TRANSACTION");
        }

        RouterHash rh = info.getIdentity().getHash();
        
        const ByteArray& encKey = info.getIdentity().getEncryptionKey();
        const ByteArray& sigKey = info.getIdentity().getSigningKey();
        const ByteArray& cert   = info.getIdentity().getCertificate().serialize();
        const ByteArray& pub    = info.getPublished().serialize();
        const ByteArray& sig    = info.getSignature();

        const std::string sql1 ="INSERT OR REPLACE INTO routers(id, encryption_key, signing_key, certificate, published, signature) VALUES(?, ?, ?, ?, ?, ?)";
        const std::string sql2 = "INSERT OR REPLACE INTO router_addresses(router_id, \"index\", cost, expiration, transport) VALUES(?, ?, ?, ?, ?)";
        const std::string sql3 = "INSERT OR REPLACE INTO router_address_options(router_id, \"index\", name, value) VALUES(?, ?, ?, ?)";
        const std::string sql4 = "INSERT OR REPLACE INTO router_options(router_id, name, value) VALUES(?, ?, ?)";
        {
    
            auto trans1 = query(sql1);
            
            trans1.bind(1, rh.data(), rh.size());
            trans1.bind(2, encKey.data(), encKey.size());
            trans1.bind(3, sigKey.data(), sigKey.size());
            trans1.bind(4, cert.data(), cert.size());
            trans1.bind(5, pub.data(), pub.size());
            trans1.bind(6, sig.data(), sig.size());
        
            if(trans1.step() != SQLITE_DONE) throw SQLError(sql1);

            int i = 0;
            for(auto & a : info) {
                std::string istr = std::to_string(i);
        
                auto trans2 = query(sql2);

                trans2.bind(1, rh.data(), rh.size());
                trans2.bind(2, istr);
                trans2.bind(3, (int) a.getCost());
                
                ByteArray expBytes =  a.getExpiration().serialize();
                trans2.bind(4, expBytes.data(), expBytes.size());
                trans2.bind(5, a.getTransport());
                
                if(trans2.step() != SQLITE_DONE) throw SQLError(sql2);
                
                for(auto & o : a.getOptions()) {
                    auto trans3 = query(sql3);
                    trans3.bind(1, rh.data(), rh.size());
                    trans3.bind(2, istr);
                    trans3.bind(3, o.first);
                    trans3.bind(4, o.second);
                    if(trans3.step() != SQLITE_DONE) throw SQLError(sql3);
                }
                i++;
            }

            for(auto& o: info.getOptions()) {
                auto trans2 = query(sql4);
                trans2.bind(1, rh.data(), rh.size());
                trans2.bind(2, o.first);
                trans2.bind(3, o.second);
                if(trans2.step() != SQLITE_DONE) throw SQLError(sql4);
            }
        }

        if(transaction) {
            db_exec("COMMIT TRANSACTION");
            m_mutex.unlock();
        }        
    }

    std::forward_list<RouterHash> Database::getAllHashes()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::forward_list<RouterHash> hashes;

        const std::string sql = "SELECT id FROM routers";
        auto trans = query(sql);
        while(trans.step() == SQLITE_ROW) {
            unsigned char * bytes = (unsigned char*) trans.get_blob(0);
            int size = trans.get_bytes(0);
            RouterHash rh;
            std::copy(bytes, bytes + size, rh.begin());
            hashes.push_front(rh);
        }
        return hashes;
    }

    void Database::db_exec(std::string const & sql)
    {
        sqlite3_exec(m_db, sql.c_str(), NULL, NULL, NULL);
    }
}
