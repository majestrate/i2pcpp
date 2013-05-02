#ifndef TUNNELTUNNELMANAGER_H
#define TUNNELTUNNELMANAGER_H

#include <map>
#include <list>
#ifndef USE_CLANG
#include <mutex>
#endif
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
			void mainloop();
		private:
			RouterContext &m_ctx;

			std::map<uint32_t, TunnelStatePtr> m_tunnels;
#ifdef USE_CLANG
			mutable std::mutex m_tunnelsMutex;
#else
			std::mutex m_tunnelsMutex;
#endif
			i2p_logger_mt m_log;
	};
}

#endif
