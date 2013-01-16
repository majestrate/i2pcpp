#ifndef TUNNELTUNNELMANAGER_H
#define TUNNELTUNNELMANAGER_H

#include <list>

#include "../datatypes/BuildRequestRecord.h"
#include "../RouterContext.h"

namespace i2pcpp {
	class TunnelManager {
		public:
			TunnelManager(RouterContext &ctx) : m_ctx(ctx) {}

			void handleRequest(std::list<BuildRequestRecord> const &records);

		private:
			RouterContext &m_ctx;
	};
}

#endif
