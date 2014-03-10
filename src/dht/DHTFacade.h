/**
 * @file DHTFacade.h
 * @brief Implements the DHT facade class, which allows easy use of the DHT.
 */
#ifndef _DHTFACADE_H_INCLUDE_GUARD
#define _DHTFACADE_H_INCLUDE_GUARD

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
             * Constructs from a reference to the i2pcpp::RouterContext
             */
            DHTFacade(RouterContext& ctx);
            
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
            std::shared_ptr<Kademlia> m_dht;
            SearchManager m_searchManager; 
        };
    }
}


#endif // _DHTFACADE_H_INCLUDE_GUARD
