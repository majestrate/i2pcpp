/**
 * @file DHTFacade.h
 * @brief Implements the DHT facade class, which allows easy use of the DHT.
 */
#ifndef _DHTFACADE_H_INCLUDE_GUARD
#define _DHTFACADE_H_INCLUDE_GUARD

#include <forward_list>

#include "Kademlia.h"
#include "SearchManager.h"

namespace i2pcpp {
    class RouterContext;

    namespace DHT {

        /**
         * Facade class for easy use of the DHT functionality.
         */
        class DHTFacade {

        public:
            /**
             * Constructs from a boost::asio::io_service, local router hash (where
             * lookups are relative to), and forward_list of initial RouterHashes.
             */
            DHTFacade(boost::asio::io_service &ios,
                    RouterHash const &local,
                    std::forward_list<RouterHash> const &hashes,
                    RouterContext &ctx);

            DHTFacade(const DHTFacade&) = delete;
            DHTFacade& operator=(DHTFacade&) = delete;

            /**
             * Starts a lookup operation for a given i2pcpp::RouterHash.
             * @param hash the hash of the router to lookup
             * @return true if the lookup was succesfully started, false otherwise
             */
            bool lookup(const RouterHash& hash);

            SearchManager& getSearchManager();

        private:
            Kademlia m_dht;
            SearchManager m_searchManager;
        };
    }
}


#endif // _DHTFACADE_H_INCLUDE_GUARD
