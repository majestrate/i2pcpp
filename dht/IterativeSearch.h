#ifndef DHTITERATIVESEARCH_H
#define DHTITERATIVESEARCH_H

#include "../OutboundMessageDispatcher.h"

#include "SearchMethod.h"
#include "I2PKey.h"
#include "I2PValue.h"

namespace i2pcpp {
	namespace DHT {
		class IterativeSearch : public SearchMethod<I2PKey, I2PValue> {
			public:
				IterativeSearch(I2PSuccessSignalPtr const &success, I2PFailureSignalPtr const failure, OutboundMessageDispatcher &omd);
				~IterativeSearch();

				void lookup(I2PKey const &k);

			private:
				OutboundMessageDispatcher& m_omd;
		};
	}
}

#endif
