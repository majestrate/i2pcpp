#ifndef ROUTER_H
#define ROUTER_H

#include <string>

#include "Transport.h"
#include "RouterContext.h"
#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
#include "JobQueue.h"
#include "JobRunner.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile) :
				m_db(dbFile),
				m_inMsgDispatcher(m_jobQueue),
				m_ctx(m_db, m_inMsgDispatcher, m_outMsgDispatcher) {}

			void start();
			void stop();

		private:
			Database m_db;
			JobQueue m_jobQueue;
			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;
			RouterContext m_ctx;
			JobRunnerPool m_jobRunnerPool;
			TransportPtr m_transport;
	};
}

#endif
