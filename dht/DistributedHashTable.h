#ifndef DHTDISTRIBUTEDHASHTABLE_H
#define DHTDISTRIBUTEDHASHTABLE_H

namespace i2pcpp {
	namespace DHT {
		template<typename Key, typename Value>
			class DistributedHashTable {
				public:
					DistributedHashTable() {}
					DistributedHashTable(const DistributedHashTable &) = delete;
					DistributedHashTable& operator=(DistributedHashTable &) = delete;
					virtual ~DistributedHashTable() {}

					virtual void insert(Key const &k, Value const &v) = 0;
					virtual void erase(Key const &k) = 0;
					virtual Value find(Key const &k) = 0;
			};
	}
}

#endif
