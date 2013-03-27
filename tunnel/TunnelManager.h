#ifndef TUNNELTUNNELMANAGER_H
#define TUNNELTUNNELMANAGER_H

#include <list>

#include "../datatypes/BuildRequestRecord.h"

namespace i2pcpp {
	class RouterContext;

	class TunnelManager {
		public:
			TunnelManager(RouterContext &ctx);

			void handleRequest(std::list<BuildRequestRecord> const &records);

		private:
			RouterContext &m_ctx;
	};
}

#endif
