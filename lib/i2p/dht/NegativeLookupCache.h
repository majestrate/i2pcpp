/**
 * @file NegativeLookupCache.h
 * @brief Defines the i2pcpp::DHT::NegativeLookupCache type.
 */
#ifndef _NEGATIVELOOKUPCACHE_H_INCLUDE_GUARD
#define _NEGATIVELOOKUPCACHE_H_INCLUDE_GUARD

#include <mutex>
#include <set>

#include <boost/asio.hpp>

#include "Kademlia.h"

namespace i2pcpp {
    namespace DHT {

        /**
         * Provides a cache to store keys that could not be looked up previously.
         * If a new lookup is attempted, it can be made to immediately fail if the
         *  cache contains the key. 
         * @note the class is designed to be thread-safe
         */
        class NegativeLookupCache {

            /**
             * Defines an entry of the i2pcpp::NegativeLookupCache.
             */
            struct Entry {
                Kademlia::key_type key;
                boost::posix_time::ptime time;

                Entry() = default;
                Entry(const Kademlia::key_type& k);
                Entry(const Kademlia::key_type& k, const boost::posix_time::ptime& t);

                /**
                 * Two entries are considered equal if they share keys.
                 */
                bool operator==(const Entry& rh) const;

                /**
                 * Compares the keys lexicographically.
                 */
                bool operator<(const Entry& rh) const;

                /**
                 * Compares the keys lexicographically.
                 */
                bool operator>(const Entry& rh) const;

                /**
                 * Compares the keys lexicographically.
                 */
                bool operator<=(const Entry& rh) const;

                /**
                 * Compares the keys lexicographically.
                 */
                bool operator>=(const Entry& rh) const;
            };
        public:

            /**
             * @param ios the IO service to run the timer on
             * @param lifetime the lifetime of a key in the cache 
             */
            NegativeLookupCache(
                boost::asio::io_service& ios,
                const boost::posix_time::time_duration& lifetime
            );

            /**
             * Checks whether a given key occurs in the cache.
             * @param key the key to check the presence of 
             * @return true if the cache contains the given key, false otherwise
             */
            bool contains(const Kademlia::key_type& key) const; 

            /**
             * Insert a key into the cache. 
             * @param key the key to insert
             */
            void insert(const Kademlia::key_type& key);

            /**
             * Callback function of the deadline timer.
             * Cleans up the cache by removing outdated entries. 
             * An entry is considered to be outdated if its lifetime has expired. 
             * Restarts the deadline timer.
             */
            void cleanup(const boost::system::error_code& e);

        private:
            /**
             * Starts the deadline timer.
             */
            void startTimer();

            std::set<Entry> m_cache;
            boost::posix_time::time_duration m_lifetime;
            boost::asio::deadline_timer m_timer;
            mutable std::mutex m_cacheMutex;
        };


    }
}


#endif // _NEGATIVELOOKUPCACHE_H_INCLUDE_GUARD
