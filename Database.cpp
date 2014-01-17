/**
 * @file Database.cpp
 * @brief Implements Database.h
 */
#include "Database.h"

#include <botan/auto_rng.h>
#include <botan/elgamal.h>
#include <botan/pkcs8.h>
#include <botan/dsa.h>

#include <boost/tokenizer.hpp>

#include "util/Base64.h"
#include "util/I2PDH.h"
#include "util/make_unique.h"

extern uint8_t _binary_schema_sql_start[];
extern uintptr_t _binary_schema_sql_size[];

namespace i2pcpp {
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

    }

    void Database::createDb(std::string const &file)
    {
        try {
            sqlite::connection conn(file);

            conn.exec("PRAGMA foreign_keys=ON");

            {
                sqlite::transaction_guard<> t(conn);

                std::string schema((char *)_binary_schema_sql_start, (uintptr_t)_binary_schema_sql_size);

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
        auto q = m_conn->make_query("SELECT value FROM config WHERE name = ?");
        *q << name;

        std::string value;
        q->step() >> value;
        return value;
    }

    void Database::setConfigValue(std::string const &name, std::string const &value)
    {
        *m_conn->make_command("INSERT OR REPLACE INTO config (name, value) VALUES (?, ?)") << name << value;
    }

    RouterHash Database::getRandomRouter()
    {
        // SELECT router_id FROM router_options WHERE router_options.name='caps' AND router_options.value LIKE '%f%' ORDER BY RANDOM() LIMIT 1;
        auto q = m_conn->make_query("SELECT id FROM routers ORDER BY RANDOM() LIMIT 1");

        std::string rhStr;
        q->step() >> rhStr;
        return toRouterHash(Base64::decode(rhStr));
    }

    bool Database::routerExists(RouterHash const &routerHash)
    {
        auto q = m_conn->make_query("SELECT COUNT(id) AS count FROM routers WHERE id = ?");
        *q << Base64::encode(routerHash);

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
        auto q = m_conn->make_query("SELECT encryption_key, signing_key, certificate, published, signature FROM routers WHERE id = ?");
        *q << routerHash;

        std::string encryptionKeyStr, signingKeyStr, certificateStr, publishedStr, signatureStr;
        q->step() >> encryptionKeyStr >> signingKeyStr >> certificateStr >> publishedStr >> signatureStr;
        ByteArray encryptionKey, signingKey, certificate, published, signature;

        std::istringstream(encryptionKeyStr) >> encryptionKey;
        std::istringstream(signingKeyStr) >> signingKey;
        std::istringstream(certificateStr) >> certificate;
        std::istringstream(publishedStr) >> published;
        std::istringstream(signatureStr) >> signature;

        Mapping router_options;

        q = m_conn->make_query("SELECT name, value FROM router_options WHERE router_id = ?");
        *q << routerHash;

        while(auto r = q->step()) {
            std::string name, value;
            r >> name >> value;
            router_options.setValue(name, value);
        }

        auto pubItr = published.cbegin();
        auto pubEndItr = published.cend();
        auto certItr = certificate.cbegin();
        auto certEndItr = certificate.cend();
        RouterInfo ri(RouterIdentity(encryptionKey, signingKey, Certificate(certItr, certEndItr)), Date(pubItr, pubEndItr), router_options, signature);

        q = m_conn->make_query("SELECT \"index\", cost, expiration, transport FROM router_addresses WHERE router_id = ?");
        *q << routerHash;

        while(auto r = q->step()) {
            int index, cost;
            std::string expirationStr, transport;

            r >> index >> cost >> expirationStr >> transport;

            ByteArray expiration;
            std::istringstream(expirationStr) >> expiration;

            auto q2 = m_conn->make_query("SELECT name, value FROM router_address_options WHERE router_id = ? AND \"index\" = ? ORDER BY name ASC");
            *q2 << routerHash << index;

            Mapping address_options;
            while(auto r2 = q2->step()) {
                std::string name, value;
                r2 >> name >> value;
                address_options.setValue(name, value);
            }

            auto expItr = expiration.cbegin();
            auto expEndItr = expiration.cend();
            RouterAddress ra(cost, Date(expItr, expEndItr), transport, address_options);
            ri.addAddress(ra);
        }

        return ri;
    }

    void Database::deleteRouter(RouterHash const &rh)
    {
        {
            sqlite::transaction_guard<> t(*m_conn);

            *m_conn->make_command("DELETE FROM profiles WHERE router_id = ?") << rh << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_address_options WHERE router_id = ?") << rh << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_addresses WHERE router_id = ?") << rh << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_options WHERE router_id = ?") << rh << sqlite::exec;
            *m_conn->make_command("DELETE FROM routers WHERE id = ?") << rh << sqlite::exec;

            t.commit();
        }
    }

    void Database::deleteAllRouters()
    {
        {
            sqlite::transaction_guard<> t(*m_conn);

            *m_conn->make_command("DELETE FROM profiles") << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_address_options") << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_addresses") << sqlite::exec;
            *m_conn->make_command("DELETE FROM router_options") << sqlite::exec;
            *m_conn->make_command("DELETE FROM routers") << sqlite::exec;

            t.commit();
        }
    }

    void Database::setRouterInfo(std::vector<RouterInfo> const &routers)
    {
        {
            sqlite::transaction_guard<> t(*m_conn);

            for(auto& r: routers)
                setRouterInfo(r);

            t.commit();
        }
    }

    void Database::setRouterInfo(RouterInfo const &info)
    {
        {
            sqlite::recursive_transaction t(*m_conn);

            t.begin();

            std::string rh = Base64::encode(info.getIdentity().getHash());

            const ByteArray& encKey = info.getIdentity().getEncryptionKey();
            const ByteArray& sigKey = info.getIdentity().getSigningKey();
            const ByteArray& cert   = info.getIdentity().getCertificate().serialize();
            const ByteArray& pub    = info.getPublished().serialize();
            const ByteArray& sig    = info.getSignature();

            *m_conn->make_command("INSERT OR REPLACE INTO routers(id, encryption_key, signing_key, certificate, published, signature) VALUES(?, ?, ?, ?, ?, ?)") << rh << encKey << sigKey << cert << pub << sig << sqlite::exec;

            int i = 0;
            for(auto& a: info) {
                std::string istr = std::to_string(i);
                *m_conn->make_command("INSERT OR REPLACE INTO router_addresses(router_id, \"index\", cost, expiration, transport) VALUES(?, ?, ?, ?, ?)") << rh << istr << (int)a.getCost() << a.getExpiration().serialize() << a.getTransport() << sqlite::exec;

                for(auto& o: a.getOptions())
                    *m_conn->make_command("INSERT OR REPLACE INTO router_address_options(router_id, \"index\", name, value) VALUES(?, ?, ?, ?)") << rh << istr << o.first << o.second << sqlite::exec;
                i++;
            }

            for(auto& o: info.getOptions())
                *m_conn->make_command("INSERT OR REPLACE INTO router_options(router_id, name, value) VALUES(?, ?, ?)") << rh << o.first << o.second << sqlite::exec;

            t.commit();
        }
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
