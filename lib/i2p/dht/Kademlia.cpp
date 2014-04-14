/**
 * @file Kademlia.cpp
 * @brief Implements Kademlia.h
 */
#include "Kademlia.h"

#include <ctime>
#include <utility>

#include <botan/pipe.h>
#include <botan/lookup.h>

namespace i2pcpp {
    namespace DHT {
        Kademlia::Kademlia(Kademlia::key_type const &reference) :
            m_ref(reference) {}

        void Kademlia::insert(Kademlia::key_type const &k, Kademlia::value_type const &v)
        {
            m_table.insert(std::make_pair(getBucket(k), v));
        }

        void Kademlia::erase(map_type::const_iterator itr)
        {
            m_table.erase(itr);
        }

        Kademlia::result_type Kademlia::find(Kademlia::key_type const &k) const
        {
            return m_table.equal_range(getBucket(k));
        }

        Kademlia::result_type Kademlia::find(Kademlia::key_type const &k, std::size_t c) const
        {
            auto iters = m_table.equal_range(getBucket(k));
            if(std::distance(iters.first, iters.second) < c)
                return iters;
            else
                return std::make_pair(iters.first, std::next(iters.first, c));
        }

        void Kademlia::setReference(Kademlia::key_type const &reference)
        {
            m_ref = reference;
        }

        Kademlia::key_type Kademlia::makeKey(RouterHash const &rh)
        {
            Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
            hashPipe.start_msg();

            hashPipe.write(rh.data(), rh.size());

            std::time_t t = std::time(nullptr);
            unsigned char time[9];
            std::strftime((char *)time, 9, "%Y%m%d", std::gmtime(&t));
            hashPipe.write(time, 8);

            hashPipe.end_msg();

            Kademlia::key_type key;
            hashPipe.read(key.data(), KEY_SIZE);

            return key;
        }

        size_t Kademlia::getBucket(Kademlia::key_type const &k) const
        {
            std::array<unsigned char, KEY_SIZE> distance;
            std::transform(k.cbegin(), k.cend(), m_ref.cbegin(), distance.begin(), std::bit_xor<unsigned char>());

            // http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn

            static const int MultiplyDeBruijnBitPosition[32] =
            {
                0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
                8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
            };

            size_t bucket = NUM_BUCKETS - 1;

            for(uint8_t v: distance) {
                if(!v) {
                    bucket -= 8;
                    continue;
                }

                v |= v >> 1;
                v |= v >> 2;
                v |= v >> 4;
                v |= v >> 8;
                v |= v >> 16;

                // bucket -= 8 - log2(v), or the amount of leftmost bits set to 1
                bucket -= 8 - (MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27]);

                break;
            }

            return bucket;
        }
    }
}
