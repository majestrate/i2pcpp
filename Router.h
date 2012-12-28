#ifndef ROUTER_H
#define ROUTER_H

#include <string>

#include "ssu/UDPTransport.h"

#include "RouterContext.h"
#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "JobQueue.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile) :
				m_db(dbFile),
				m_ctx(m_db),
				m_inMsgDispatcher(m_jobQueue),
				m_transport(m_ctx, m_inMsgDispatcher) {}

			void start();
			void stop();

		private:
			Database m_db;
			RouterContext m_ctx;
			InboundMessageDispatcher m_inMsgDispatcher;
			JobQueue m_jobQueue;
			SSU::UDPTransport m_transport;
	};
}

#endif
