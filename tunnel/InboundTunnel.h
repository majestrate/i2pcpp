#ifndef INBOUNDTUNNEL_H
#define INBOUNDTUNNEL_H

#include <list>
#include <vector>

#include "../RouterContext.h"
#include "../datatypes/RouterHash.h"
#include "../datatypes/BuildRequestRecord.h"

namespace i2pcpp {
	class InboundTunnel {
		public:
			InboundTunnel(RouterContext &ctx, std::list<RouterHash> &hops);

			void sendRequest();

		private:
			RouterContext& m_ctx;

			std::list<BuildRequestRecord> m_records;
			RouterHash m_inboundGateway;
	};
}

#endif
