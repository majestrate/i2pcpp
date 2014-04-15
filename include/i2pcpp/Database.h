/**
 * @file Database.h
 * @brief Defines the i2pcpp::Database type.
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <i2pcpp/datatypes/ByteArray.h>
#include <i2pcpp/datatypes/RouterHash.h>

#include <memory>
#include <string>
#include <forward_list>
#include <unordered_map>

namespace sqlite {
    class connection;
    class command;
    class query;

    namespace detail {
        class basic_statement;
    }
}

namespace i2pcpp {
    class RouterInfo;

    /**
     * An utility wrapper for the sqlite3 functionality.
     */
    class Database {
        public:
            /**
             * Constructs from a database file given by its name.
             * @param file the name of the database file
             */
            Database(std::string const &file);
            Database(const Database &) = delete;
            Database& operator=(Database &) = delete;

            /**
             * Creates a new database file.
             * @param file the name of the database file to be created
             */
            static void createDb(std::string const &file);

            /**
             * @return the value of the configuration field \a name
             */
            std::string getConfigValue(std::string const &name);

            /**
             * Sets the value of the configuration field \a name to \a value.
             */
            void setConfigValue(std::string const &name, std::string const &value);

            /**
             * Gets the blob value of the configuration field \a name.
             */
            ByteArray getConfigBlob(std::string const &name);

            /**
             * @return the i2pcpp::RouterHash of a randomly selected router
             */
            RouterHash getRandomRouter();

            /**
             * @return true if we know the router with i2pcpp::RouterHash
             *  \a routerHash, false otherwise
             */
            bool routerExists(RouterHash const &routerHash);

            /**
             * @return the i2pcpp::RouterInfo associated with a router given
             *  by its a routerHash given as a std::string.
             */
            RouterInfo getRouterInfo(std::string const &routerHash);

            /**
             * @return the i2pcpp::RouterInfo associated with a router given
             *  by its i2pcpp::RouterHash \a routerHash.
             */
            RouterInfo getRouterInfo(RouterHash const &routerHash);

            /**
             * Deletes the router gien by its i2pcpp::RouterHash \a hash.
             */
            void deleteRouter(RouterHash const &hash);

            /**
             * Deletes (forgets) all known routers.
             */
            void deleteAllRouters();

            /**
             * Inserts or replaces a std::vector of i2pcpp::RouterInfo objects.
             */
            void setRouterInfo(std::vector<RouterInfo> const &routers);

            /**
             * Inserts or replaces an i2pcpp::RouterInfo object \a info.
             */
            void setRouterInfo(RouterInfo const &info);

            /**
             * @return a list of the i2pcpp::RouterHash objects of all known
             *  routers
             */
            std::forward_list<RouterHash> getAllHashes();

        private:
            std::shared_ptr<sqlite::connection> m_conn;

            static std::unordered_map<std::string, std::shared_ptr<sqlite::command>> commands;
            static std::unordered_map<std::string, std::shared_ptr<sqlite::query>> queries;
    };
}

#endif
