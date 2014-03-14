/**
 * @file NegativeLookupCache.cpp
 * @brief Implements NegativeLookupCache.h 
 */

#include "NegativeLookupCache.h"

#include <boost/bind.hpp>

namespace i2pcpp {
    namespace DHT {

        NegativeLookupCache::Entry::Entry(const Kademlia::key_type& k)
            : key(k), time()
        {
        }

        NegativeLookupCache::Entry::Entry(const Kademlia::key_type& k,
         const boost::posix_time::ptime& t)
            : key(k), time(t)
        {
        }

        bool NegativeLookupCache::Entry::operator==(const Entry& rh) const
        {
            return key == rh.key;
        }

        bool NegativeLookupCache::Entry::operator<(const Entry& rh) const
        {
            return key < rh.key;
        }

        bool NegativeLookupCache::Entry::operator>(const Entry& rh) const
        {
            return key > rh.key;
        }

        bool NegativeLookupCache::Entry::operator<=(const Entry& rh) const
        {
            return key <= rh.key;
        }

        bool NegativeLookupCache::Entry::operator>=(const Entry& rh) const
        {
            return key >= rh.key;
        }

        NegativeLookupCache::NegativeLookupCache(
            boost::asio::io_service& ios,
            const boost::posix_time::time_duration& lifetime 
        ) :
            m_cache(),
            m_lifetime(lifetime),
            m_timer(ios, m_lifetime)
        {
            startTimer();
        } 

        bool NegativeLookupCache::contains(const Kademlia::key_type& key) const 
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            return m_cache.count(Entry(key));
        }

        void NegativeLookupCache::insert(const Kademlia::key_type& key)
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_cache.insert(
                Entry(key, boost::posix_time::second_clock::local_time())
            );
        }

        void NegativeLookupCache::cleanup(const boost::system::error_code& e)
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            for(auto it = m_cache.begin(); it != m_cache.end(); ++it) {
                if(it->time - boost::posix_time::second_clock::local_time() >= m_lifetime)
                    it = m_cache.erase(it);
            }
            startTimer();
        }

        void NegativeLookupCache::startTimer()
        {
            m_timer.expires_from_now(m_lifetime);

            m_timer.async_wait(boost::bind(
                &NegativeLookupCache::cleanup, this, boost::asio::placeholders::error
            ));
        }
    }
}
