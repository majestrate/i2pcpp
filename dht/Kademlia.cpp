#include "Kademlia.h"

#include <ctime>

#include <botan/pipe.h>
#include <botan/lookup.h>

namespace i2pcpp {
	namespace DHT {
		Kademlia::Kademlia(KademliaKey const &reference) :
			DistributedHashTable(),
			m_ref(reference) {}

		Kademlia::~Kademlia() {}

		void Kademlia::insert(KademliaKey const &k, KademliaValue const &v)
		{
			auto& bucket = m_table[getBucket(k)];

			if(bucket.size() >= K_VALUE) {
				auto last = bucket.end();
				--last;

				bucket.erase(last);
			}

			bucket[k] = v;
		}

		void Kademlia::erase(KademliaKey const &k)
		{
			auto& bucket = m_table[getBucket(k)];

			bucket.erase(k);
		}

		KademliaValue Kademlia::find(KademliaKey const &k)
		{
			size_t bucketNum = getBucket(k);
			auto& bucket = m_table[bucketNum];

			if(bucket.count(k))
				return bucket[k];

			while(!bucket.size() && ++bucketNum < NUM_BUCKETS)
				bucket = m_table[bucketNum];

			if(!bucket.size()) return KademliaValue();

			return bucket.cbegin()->second;
		}

		KademliaKey Kademlia::makeKey(RouterHash const &rh)
		{
			Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
			hashPipe.start_msg();

			std::time_t t = std::time(nullptr);
			unsigned char time[8];
			std::strftime((char *)time, 8, "%Y%m%d", std::gmtime(&t));
			hashPipe.write(rh.data(), rh.size());
			hashPipe.write(time, 8);

			hashPipe.end_msg();

			KademliaKey key;
			hashPipe.read(key.data(), KEY_SIZE);

			return key;
		}

		void Kademlia::setReference(KademliaKey const &reference)
		{
			m_ref = reference;
		}

		size_t Kademlia::getBucket(KademliaKey const &k)
		{
			std::array<unsigned char, KEY_SIZE> distance;
			for(int i = 0; i < KEY_SIZE; i++)
				distance[i] = m_ref[i] ^ k[i];

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

				bucket -= 8 - (MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27]);

				break;
			}

			return bucket;
		}
	}

	std::size_t hash_value(DHT::KademliaKey const &k)
	{
		boost::hash<std::string> f;
		return f(k);
	}
}
