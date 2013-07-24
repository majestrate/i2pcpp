#ifndef TUNNELMANAGER_H
#define TUNNELMANAGER_H

#include <mutex>
#include <unordered_map>

#include <boost/asio.hpp>

#include "../datatypes/BuildRecord.h"
#include "../datatypes/BuildRequestRecord.h"
#include "../datatypes/BuildResponseRecord.h"

#include "../Log.h"

#include "Tunnel.h"
#include "TunnelHop.h"
#include "TunnelGenerator.h"

namespace i2pcpp {
	class RouterContext;

	class TunnelManager {
		public:
			TunnelManager(boost::asio::io_service &ios, RouterContext &ctx);
			TunnelManager(const TunnelManager &) = delete;
			TunnelManager& operator=(TunnelManager &) = delete;

			void begin();
			void receiveRecords(std::list<BuildRecordPtr> records);
			void receiveGatewayData(uint32_t const tunnelId, ByteArray const data);

		private:
			void callback(const boost::system::error_code &e);
			void createTunnel();

			boost::asio::io_service &m_ios;
			RouterContext &m_ctx;

			std::unordered_map<uint32_t, TunnelPtr> m_tunnels;
			std::unordered_map<uint32_t, TunnelHopPtr> m_participating;

			mutable std::mutex m_tunnelsMutex;
			mutable std::mutex m_participatingMutex;

			boost::asio::deadline_timer m_timer;

			i2p_logger_mt m_log;

			TunnelGenerator m_generator;

			uint32_t m_tunnel_count = 0;

	};
}

#endif
