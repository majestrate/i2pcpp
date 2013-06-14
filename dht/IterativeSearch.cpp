#include "IterativeSearch.h"

namespace i2pcpp {
	namespace DHT {
		IterativeSearch::IterativeSearch(I2PSuccessSignalPtr const &success, I2PFailureSignalPtr const failure, OutboundMessageDispatcher &omd) :
			SearchMethod(success, failure),
			m_omd(omd) {}

		IterativeSearch::~IterativeSearch() {}

		void IterativeSearch::lookup(I2PKey const &k)
		{
		}

		// callback
	}
}
