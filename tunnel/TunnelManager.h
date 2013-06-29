#ifndef TUNNELMANAGER_H
#define TUNNELMANAGER_H

#include <mutex>
#include <unordered_map>

#include "../datatypes/BuildRecord.h"
#include "../datatypes/BuildRequestRecord.h"
#include "../datatypes/BuildResponseRecord.h"

#include "../Log.h"

#include "Tunnel.h"
#include "TunnelHop.h"

namespace i2pcpp {
	class RouterContext;

	class TunnelManager {
		public:
			TunnelManager(RouterContext &ctx);
			TunnelManager(const TunnelManager &) = delete;
			TunnelManager& operator=(TunnelManager &) = delete;

			void receiveRecords(std::list<BuildRecord> records);

		private:
			RouterContext &m_ctx;

			std::unordered_map<uint32_t, TunnelPtr> m_tunnels;
			std::unordered_map<uint32_t, TunnelHopPtr> m_participating;

			mutable std::mutex m_tunnelsMutex;
			mutable std::mutex m_participatingMutex;

			i2p_logger_mt m_log;
	};
}

#endif
