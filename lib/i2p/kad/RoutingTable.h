/**
 * @file RoutingTable.h
 * @brief Defines the i2pcpp::Kad::RoutingTable class.
 */

#ifndef KADROUTINGTABLE_H
#define KADROUTINGTABLE_H

#include <i2pcpp/datatypes/RouterInfo.h>

#include <set>

#define KEY_SIZE 32
#define NUM_BUCKETS (KEY_SIZE * 8)
#define K_VALUE 20

namespace i2pcpp {
    namespace Kad {
        class RoutingTable {
            private:
                struct Entry {
                    StaticByteArray<32> key;
                    RouterInfo value;

                    bool operator<(const Entry& e) const;
                };

                typedef std::set<Entry> bucket_t;
                std::array<bucket_t, NUM_BUCKETS> m_buckets;
        };
    }
}

#endif
