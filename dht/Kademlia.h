#ifndef DHTKADEMLIA_H
#define DHTKADEMLIA_H

#include <map>
#include <array>
#include <memory>

#include <boost/functional/hash.hpp>

#include "../datatypes/StaticByteArray.h"
#include "../datatypes/RouterHash.h"

#include "DistributedHashTable.h"

#define KEY_SIZE 32
#define NUM_BUCKETS (KEY_SIZE * 8)
#define K_VALUE 20

namespace i2pcpp {
	namespace DHT {
		typedef StaticByteArray<KEY_SIZE> KademliaKey;
		typedef RouterHash KademliaValue;

		class Kademlia : public DistributedHashTable<KademliaKey, KademliaValue> {
			public:
				Kademlia(KademliaKey const &reference);
				~Kademlia();

				void insert(KademliaKey const &k, KademliaValue const &v);
				void erase(KademliaKey const &k);
				KademliaValue find(KademliaKey const &k);

				void setReference(KademliaKey const &reference);

				static KademliaKey makeKey(RouterHash const &rh);

			private:
				size_t getBucket(KademliaKey const &k);

				KademliaKey m_ref;

				std::array<std::map<KademliaKey, KademliaValue>, NUM_BUCKETS> m_table;
		};

		typedef std::shared_ptr<Kademlia> KademliaPtr;
	}

	std::size_t hash_value(DHT::KademliaKey const &k);
}

#endif
