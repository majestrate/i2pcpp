#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "handlers/DatabaseStore.h"
#include "handlers/DatabaseSearchReply.h"

#include "tunnel/TunnelManager.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
#include "Signals.h"
#include "RouterContext.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile) :
				m_work(m_ios),
				m_db(dbFile),
				m_ctx(m_db, m_outMsgDispatcher, m_signals),
				m_inMsgDispatcher(m_ios, m_ctx),
				m_outMsgDispatcher(m_ios),
				m_signals(m_ios),
				m_tunnelManager(m_ctx) {}

			~Router();

			void start();
			void stop();

			/* These are only temporary and will by used for
			 * testing and debugging. */
			void connect(std::string const &to);
			void databaseLookup(std::string const &to, std::string const &query);
			void createTunnel(std::list<std::string> const &hopList);

		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			Database m_db;
			RouterContext m_ctx;
			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;
			Signals m_signals;
			TransportPtr m_transport;
			TunnelManager m_tunnelManager;
	};
}

#endif
