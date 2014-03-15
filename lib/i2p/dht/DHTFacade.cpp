/**
 * @file DHTFacade.cpp
 * @brief Implements DHTFacade.h
 */
#include "DHTFacade.h"

#include "RouterContext.h"
#include <i2pcpp/datatypes/RouterIdentity.h>

#include <i2pcpp/util/make_unique.h>

namespace i2pcpp {
    namespace DHT {

        DHTFacade::DHTFacade(RouterContext& ctx) :
            m_dht(std::make_unique<DHT::Kademlia>(ctx.getIdentity()->getHash())),
            m_searchManager(ctx)
        {
            // Populate the DHT
            for(const auto& h: ctx.getDatabase()->getAllHashes())
                m_dht->insert(DHT::Kademlia::makeKey(h), h);
        }


        bool DHTFacade::lookup(const RouterHash& hash)
        {
            auto results = m_dht->find(DHT::Kademlia::makeKey(hash));
            if(std::distance(results.first, results.second) < 1)
                return false;

            m_searchManager.createSearch(hash, results);
        }

        SearchManager& DHTFacade::getSearchManager()
        {
            return m_searchManager;
        }
    }
}
