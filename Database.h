#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <forward_list>
#include <unordered_map>

#include <sqlite3.h>

#include "datatypes/ByteArray.h"
#include "datatypes/RouterInfo.h"

namespace i2pcpp {


    class sql_transaction 
    {
    public:
        sql_transaction(sqlite3_stmt * query);
        ~sql_transaction();
        void bind(int col, int data);
        void bind(int col, std::string const & data);
        void bind(int col, const void * data, uint32_t len);
        int get_int(int col);
        std::string get_str(int col);
        const void * get_blob(int col);
        int get_bytes(int col);
        int step();
    private:
        sqlite3_stmt * m_stmt;
    }; 
    
    class Database {
        public:
            Database(std::string const &file);
            Database(const Database &) = delete;
            Database& operator=(Database &) = delete;
            ~Database();

            static void createDb(std::string const &file);
            std::string getConfigValue(std::string const &name);
            void setConfigValue(std::string const &name, std::string const &value);
            ByteArray getConfigBlob(std::string const &name);
            RouterHash getRandomRouter();
            bool routerExists(RouterHash const &routerHash);
            RouterInfo getRouterInfo(std::string const &routerHash);
            RouterInfo getRouterInfo(RouterHash const &routerHash);
            void deleteRouter(RouterHash const &hash);
            void deleteAllRouters();
            void setRouterInfo(std::vector<RouterInfo> const &routers);
            void setRouterInfo(RouterInfo const &info, bool transaction = true);
            std::forward_list<RouterHash> getAllHashes();

        private:
            sqlite3 *m_db;
            std::unordered_map<std::string, sqlite3_stmt *> m_queries;
            sql_transaction query(std::string const & sql);
            void db_exec(std::string const & sql);
            mutable std::mutex m_mutex;

    };
}

#endif
