#ifndef TUNNELSTATE_H
#define TUNNELSTATE_H

#include <list>
#include <vector>

#include "../datatypes/RouterHash.h"
#include "../datatypes/BuildRequestRecord.h"

namespace i2pcpp {
	class RouterContext;

	class TunnelState {
		public:
			TunnelState(RouterContext &ctx, std::list<RouterHash> &hops);

			void sendRequest();

		private:
			RouterContext& m_ctx;

			std::list<BuildRequestRecord> m_records;
			RouterHash m_inboundGateway;
	};
}

#endif
