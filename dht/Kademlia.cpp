#include "Kademlia.h"

namespace i2pcpp {
	namespace DHT {
		Kademlia::Kademlia(OutboundMessageDispatcher &omd) :
			DistributedHashTable(),
			m_omd(omd) {}

		Kademlia::~Kademlia() {}

		void Kademlia::insert(I2PKey const &k, I2PValue const &v)
		{
		}

		void Kademlia::erase(I2PKey const &k)
		{
		}

		void Kademlia::find(I2PKey const &k)
		{
		}
	}
}
