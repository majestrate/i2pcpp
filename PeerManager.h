#ifndef PEERMANAGER_H
#define PEERMANAGER_H

#include <thread>

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "Log.h"

namespace i2pcpp {
	class RouterContext;

	class PeerManager {
		public:
			PeerManager(boost::asio::io_service &ios, RouterContext &ctx);

			void begin();

			void establishmentSuccess(const RouterHash rh, bool inbound);
			void establishmentFailure(const RouterHash rh);
			void disconnected(const RouterHash rh);

		private:
			void callback(const boost::system::error_code &e);

			boost::asio::io_service& m_ios;
			RouterContext& m_ctx;

			std::atomic<uint32_t> m_inboundEstablished;
			std::atomic<uint32_t> m_outboundEstablished;

			boost::asio::deadline_timer m_timer;

			i2p_logger_mt m_log;
	};
}

#endif
