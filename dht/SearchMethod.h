#ifndef DHTSEARCHMETHOD_H
#define DHTSEARCHMETHOD_H

#include "DistributedHashTable.h"

namespace i2pcpp {
	namespace DHT {
		template<typename Key, typename Value>
			class SearchMethod {
				public:
					typedef typename DistributedHashTable<Key, Value>::SuccessSignalPtr I2PSuccessSignalPtr;
					typedef typename DistributedHashTable<Key, Value>::FailureSignalPtr I2PFailureSignalPtr;

					SearchMethod(I2PSuccessSignalPtr const success, I2PFailureSignalPtr const failure) :
						m_successSignal(success),
						m_failureSignal(failure) {}

					virtual ~SearchMethod() {}

					virtual void lookup(Key const &k) = 0;

				protected:
					I2PSuccessSignalPtr m_successSignal;
					I2PFailureSignalPtr m_failureSignal;
			};
	}
}

#endif
