#ifndef DHTKADEMLIA_H
#define DHTKADEMLIA_H

#include <map>
#include <memory>

#include <boost/functional/hash.hpp>

#include "../datatypes/StaticByteArray.h"
#include "../datatypes/RouterHash.h"

#define KEY_SIZE 32
#define NUM_BUCKETS (KEY_SIZE * 8)
#define K_VALUE 20

namespace i2pcpp {
    namespace DHT {
        class Kademlia {
            public:
                typedef StaticByteArray<KEY_SIZE> key_type;
                typedef RouterHash value_type;
                typedef std::multimap<size_t, value_type> map_type;
                typedef std::pair<map_type::const_iterator, map_type::const_iterator> result_type;

                Kademlia(key_type const &reference);

                void insert(key_type const &k, value_type const &v);
                void erase(map_type::const_iterator itr);
                result_type find(key_type const &k) const;

                void setReference(key_type const &reference);

                static key_type makeKey(value_type const &rh);

            private:
                size_t getBucket(key_type const &k) const;

                key_type m_ref;
                map_type m_table;
        };

        typedef std::shared_ptr<Kademlia> KademliaPtr;
    }

    std::size_t hash_value(DHT::Kademlia::key_type const &k);
}

#endif
