/**
 * @file Database.cpp
 * @brief Implements Database.h
 */
#include "../../include/i2pcpp/Database.h"

#include "sqlite3cc.h"
#include "statement_guard.h"

#include <i2pcpp/util/Base64.h>
#include <i2pcpp/util/I2PDH.h>
#include <i2pcpp/util/make_unique.h>

#include <i2pcpp/datatypes/RouterInfo.h>

#include <botan/auto_rng.h>
#include <botan/elgamal.h>
#include <botan/pkcs8.h>
#include <botan/dsa.h>

#include <boost/tokenizer.hpp>

extern uint8_t _binary_schema_sql_start[];
extern uintptr_t _binary_schema_sql_size[];

namespace i2pcpp {
    std::unordered_map<std::string, std::shared_ptr<sqlite::command>> Database::commands;
    std::unordered_map<std::string, std::shared_ptr<sqlite::query>> Database::queries;

    Database::Database(std::string const &file)
    {
        try {
            m_conn = std::make_unique<sqlite::connection>(file);
            m_conn->exec("PRAGMA foreign_keys=ON");
            m_conn->exec("PRAGMA synchronous=OFF");
            m_conn->exec("PRAGMA temp_store=MEMORY");
        } catch(sqlite::sqlite_error &e) {
            throw std::runtime_error("could not open database");
        }

        Database::queries["get_config"] = m_conn->make_query("SELECT value FROM config WHERE name = ?");
        Database::queries["get_random_router"] = m_conn->make_query("SELECT id FROM routers ORDER BY RANDOM() LIMIT 1");
        Database::queries["router_exists"] = m_conn->make_query("SELECT COUNT(id) AS count FROM routers WHERE id = ?");
        Database::queries["get_router"] = m_conn->make_query("SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?");
        Database::queries["get_router_options"] = m_conn->make_query("SELECT name, value FROM router_options WHERE router_id = ?");
        Database::queries["get_router_addresses"] = m_conn->make_query("SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?");
        Database::queries["router_address_options"] = m_conn->make_query("SELECT name, value FROM router_address_options WHERE router_id = ? AND \"index\" = ? ORDER BY name ASC");

        Database::commands["set_config"] = m_conn->make_command("INSERT OR REPLACE INTO config (name, value) VALUES (?, ?)");
        Database::commands["delete_profile"] = m_conn->make_command("DELETE FROM profiles WHERE router_id = ?");
        Database::commands["delete_router_address_options"] = m_conn->make_command("DELETE FROM router_address_options WHERE router_id = ?");
        Database::commands["delete_router_addresses"] = m_conn->make_command("DELETE FROM router_addresses WHERE router_id = ?");
        Database::commands["delete_router_options"] = m_conn->make_command("DELETE FROM router_options WHERE router_id = ?");
        Database::commands["delete_router"] = m_conn->make_command("DELETE FROM routers WHERE id = ?");
        Database::commands["truncate_profiles"] = m_conn->make_command("DELETE FROM profiles");
        Database::commands["truncate_router_address_options"] = m_conn->make_command("DELETE FROM router_address_options");
        Database::commands["truncate_router_addresses"] = m_conn->make_command("DELETE FROM router_addresses");
        Database::commands["truncate_router_options"] = m_conn->make_command("DELETE FROM router_options");
        Database::commands["truncate_router_options"] = m_conn->make_command("DELETE FROM routers");
        Database::commands["insert_router"] = m_conn->make_command("INSERT OR REPLACE INTO routers(id, encryption_key, signing_key, certificate, published, signature) VALUES(?, ?, ?, ?, ?, ?)");
        Database::commands["insert_router_address"] = m_conn->make_command("INSERT OR REPLACE INTO router_addresses(router_id, \"index\", cost, expiration, transport) VALUES(?, ?, ?, ?, ?)");
        Database::commands["insert_router_address_option"] = m_conn->make_command("INSERT OR REPLACE INTO router_address_options(router_id, \"index\", name, value) VALUES(?, ?, ?, ?)");
        Database::commands["insert_router_option"] = m_conn->make_command("INSERT OR REPLACE INTO router_options(router_id, name, value) VALUES(?, ?, ?)");
    }

    void Database::createDb(std::string const &file)
    {
        try {
            sqlite::connection conn(file);

            conn.exec("PRAGMA foreign_keys=ON");

            {
                sqlite::transaction_guard<> t(conn);
                std::string schema((const char *)_binary_schema_sql_start);
                
                boost::char_separator<char> sep(";");
                boost::tokenizer<boost::char_separator<char>> tok(schema, sep);

                auto itr = tok.begin();
                for(auto s: tok) {
                    if(s == "\n" || s.size() < 2) continue;

                    conn.exec(s);
                }

                Botan::AutoSeeded_RNG rng;
                Botan::DSA_PrivateKey dsa_key(rng, DH::getGroup());
                Botan::ElGamal_PrivateKey elg_key(rng, Botan::DL_Group("modp/ietf/2048"));
                std::string elg_string = Botan::PKCS8::PEM_encode(elg_key);
                std::string dsa_string = Botan::PKCS8::PEM_encode(dsa_key);

                *conn.make_command("INSERT OR REPLACE INTO config (name, value) VALUES ('private_encryption_key', ?)") << elg_string << sqlite::exec;

                *conn.make_command("INSERT OR REPLACE INTO config (name, value) VALUES ('private_signing_key', ?)") << dsa_string << sqlite::exec;

                t.commit();
            }
        } catch(sqlite::sqlite_error &e) {
            throw std::runtime_error("could not open database");
        }
    }

    std::string Database::getConfigValue(std::string const &name)
    {
        auto q = Database::queries["get_config"];
        statement_guard sg(q, name);

        sqlite::row r = q->step();
        if(!r)
            throw std::runtime_error("config value not found: " + name);

        std::string value;
        r >> value;
        return value;
    }

    void Database::setConfigValue(std::string const &name, std::string const &value)
    {
        statement_guard sg(Database::commands["set_config"], name, value, sqlite::exec);
    }

    RouterHash Database::getRandomRouter()
    {
        // SELECT router_id FROM router_options WHERE router_options.name='caps' AND router_options.value LIKE '%f%' ORDER BY RANDOM() LIMIT 1;
        auto q = Database::queries["get_random_router"];
        statement_guard sg(q);

        sqlite::row r = q->step();
        if(!r)
            throw std::runtime_error("couldn't fetch random router");

        std::string rhStr;
        r >> rhStr;
        return toRouterHash(Base64::decode(rhStr));
    }

    bool Database::routerExists(RouterHash const &routerHash)
    {
        auto q = Database::queries["router_exists"];
        statement_guard sg(q, Base64::encode(routerHash));

        bool exists;
        q->step() >> exists;
        return exists;
    }

    RouterInfo Database::getRouterInfo(RouterHash const &routerHash)
    {
        return getRouterInfo(Base64::encode(routerHash));
    }

    RouterInfo Database::getRouterInfo(std::string const &routerHash)
    {
        sqlite::transaction_guard<> t(*m_conn);

        // *** First query
        auto q1 = Database::queries["get_router"];
        statement_guard sg1(q1, routerHash);

        sqlite::row r1 = q1->step();
        if(!r1)
            throw std::runtime_error("router not found");

        std::string encryptionKeyStr, signingKeyStr, certificateStr, publishedStr, signatureStr;
        r1 >> encryptionKeyStr >> signingKeyStr >> certificateStr >> publishedStr >> signatureStr;
        ByteArray encryptionKey, signingKey, certificate, published, signature;

        std::istringstream(encryptionKeyStr) >> encryptionKey;
        std::istringstream(signingKeyStr) >> signingKey;
        std::istringstream(certificateStr) >> certificate;
        std::istringstream(publishedStr) >> published;
        std::istringstream(signatureStr) >> signature;

        // *** Second query
        auto q2 = Database::queries["get_router_options"];
        statement_guard sg2(q2, routerHash);

        Mapping router_options;
        while(auto r2 = q2->step()) {
            std::string name, value;
            r2 >> name >> value;
            router_options.setValue(name, value);
        }

        auto pubItr = published.cbegin();
        auto pubEndItr = published.cend();
        auto certItr = certificate.cbegin();
        auto certEndItr = certificate.cend();
        RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr, certEndItr)), Date(pubItr, pubEndItr), router_options, signature);

        // *** Third query
        auto q3 = Database::queries["get_router_addresses"];
        statement_guard sg3(q3, routerHash);

        while(auto r3 = q3->step()) {
            int index, cost;
            std::string expirationStr, transport;

            r3 >> index >> cost >> expirationStr >> transport;

            ByteArray expiration;
            std::istringstream(expirationStr) >> expiration;

            // *** Fourth query
            auto q4 = Database::queries["router_address_options"];
            statement_guard sg4(q4, routerHash, index);

            Mapping address_options;
            while(auto r4 = q4->step()) {
                std::string name, value;
                r4 >> name >> value;
                address_options.setValue(name, value);
            }

            auto expItr = expiration.cbegin();
            auto expEndItr = expiration.cend();
            RouterAddress ra(cost, Date(expItr, expEndItr), transport, address_options);
            ri.addAddress(ra);
        }

        t.commit();

        return ri;
    }

    void Database::deleteRouter(RouterHash const &rh)
    {
        sqlite::transaction_guard<> t(*m_conn);

        statement_guard sg1(Database::commands["delete_profile"], rh, sqlite::exec);
        statement_guard sg2(Database::commands["delete_router_address_options"], rh, sqlite::exec);
        statement_guard sg3(Database::commands["delete_router_addresses"], rh, sqlite::exec);
        statement_guard sg4(Database::commands["delete_router_options"], rh, sqlite::exec);
        statement_guard sg5(Database::commands["delete_router"], rh, sqlite::exec);

        t.commit();
    }

    void Database::deleteAllRouters()
    {
        sqlite::transaction_guard<> t(*m_conn);

        statement_guard sg1(Database::commands["truncate_profiles"], sqlite::exec);
        statement_guard sg2(Database::commands["truncate_router_address_options"], sqlite::exec);
        statement_guard sg3(Database::commands["truncate_router_addresses"], sqlite::exec);
        statement_guard sg4(Database::commands["truncate_router_options"], sqlite::exec);
        statement_guard sg5(Database::commands["truncate_router_options"], sqlite::exec);

        t.commit();
    }

    void Database::setRouterInfo(std::vector<RouterInfo> const &routers)
    {
        sqlite::transaction_guard<> t(*m_conn);

        for(auto& r: routers)
            setRouterInfo(r);

        t.commit();
    }

    void Database::setRouterInfo(RouterInfo const &info)
    {
        sqlite::recursive_transaction t(*m_conn);

        t.begin();

        std::string rh = Base64::encode(info.getIdentity().getHash());

        const ByteArray& encKey = info.getIdentity().getEncryptionKey();
        const ByteArray& sigKey = info.getIdentity().getSigningKey();
        const ByteArray& cert   = info.getIdentity().getCertificate().serialize();
        const ByteArray& pub    = info.getPublished().serialize();
        const ByteArray& sig    = info.getSignature();

        statement_guard sg1(Database::commands["insert_router"], rh, encKey, sigKey, cert, pub, sig, sqlite::exec);

        int i = 0;
        for(auto& a: info) {
            std::string istr = std::to_string(i);
            statement_guard sg2(Database::commands["insert_router_address"], rh, istr, (int)a.getCost(), a.getExpiration().serialize(), a.getTransport(), sqlite::exec);

            for(auto& o: a.getOptions())
                statement_guard sg3(Database::commands["insert_router_address_option"], rh, istr, o.first, o.second, sqlite::exec);
            i++;
        }

        for(auto& o: info.getOptions())
            statement_guard sg4(Database::commands["insert_router_option"], rh, o.first, o.second, sqlite::exec);

        t.commit();
    }

    std::forward_list<RouterHash> Database::getAllHashes()
    {
        std::forward_list<RouterHash> hashes;

        auto q = m_conn->make_query("SELECT id FROM routers");

        while(auto r = q->step()) {
            std::string rhStr;
            r >> rhStr;
            hashes.push_front(toRouterHash(Base64::decode(rhStr)));
        }

        return hashes;
    }
}
