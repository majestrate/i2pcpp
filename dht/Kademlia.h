#ifndef DHTKADEMLIA_H
#define DHTKADEMLIA_H

#include <map>
#include <array>
#include <memory>

#include "../datatypes/RouterHash.h"

#include "DistributedHashTable.h"

#define KEY_SIZE 32
#define NUM_BUCKETS (KEY_SIZE * 8)
#define K_VALUE 20

namespace i2pcpp {
	namespace DHT {
		typedef std::array<unsigned char, KEY_SIZE> KademliaKey;
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
}

namespace std {
	template<>
	struct hash<i2pcpp::DHT::KademliaKey> {
		public:
			size_t operator()(const i2pcpp::DHT::KademliaKey &k) const
			{
				hash<string> f;
				return f(string(k.cbegin(), k.cend()));
			}
	};
}
#endif
