#ifndef TUNNELTUNNELMANAGER_H
#define TUNNELTUNNELMANAGER_H

#include <map>
#include <list>

#include "../datatypes/BuildRequestRecord.h"

#include "../Log.h"

#include "TunnelState.h"

namespace i2pcpp {
	class RouterContext;

	class TunnelManager {
		public:
			TunnelManager(RouterContext &ctx);

			void createTunnel(bool inbound);
			void handleRequest(std::list<BuildRequestRecord> const &records);

		private:
			RouterContext &m_ctx;

			std::map<uint32_t, TunnelStatePtr> m_tunnels;

			i2p_logger_mt m_log;
	};
}

#endif
