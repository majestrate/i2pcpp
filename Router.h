#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "handlers/DatabaseStore.h"
#include "handlers/DatabaseSearchReply.h"

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
				m_inMsgDispatcher(m_ios, m_ctx),
				m_outMsgDispatcher(m_ios),
				m_signals(m_ios),
				m_ctx(m_db, m_outMsgDispatcher, m_signals) {}

			~Router();

			void start();
			void stop();

			/* These are only temporary and will by used for
			 * testing and debugging. */
			void databaseLookup(std::string const &to, std::string const &query);
			void createTunnel(std::string const &to);

		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			Database m_db;
			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;
			Signals m_signals;
			RouterContext m_ctx;
			TransportPtr m_transport;
	};
}

#endif
