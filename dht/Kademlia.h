#ifndef DHTKADEMLIA_H
#define DHTKADEMLIA_H

#include <unordered_map>

#include "../OutboundMessageDispatcher.h"

#include "I2PKey.h"
#include "I2PValue.h"
#include "SearchMethod.h"
#include "DistributedHashTable.h"

namespace i2pcpp {
	namespace DHT {
		class Kademlia : public DistributedHashTable<I2PKey, I2PValue> {
			public:
				typedef std::shared_ptr<SearchMethod<I2PKey, I2PValue>> I2PSearchMethodPtr;

				Kademlia(OutboundMessageDispatcher &omd);
				~Kademlia();

				void insert(I2PKey const &k, I2PValue const &v);
				void erase(I2PKey const &k);
				void find(I2PKey const &k);

			private:
				OutboundMessageDispatcher& m_omd;

				std::unordered_map<I2PKey, I2PSearchMethodPtr> m_lookups;
		};
	}
}

#endif
