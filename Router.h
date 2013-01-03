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
				m_outMsgDispatcher(m_transport),
				m_ctx(m_db, m_inMsgDispatcher, m_outMsgDispatcher) {}

			void start();
			void stop();

		private:
			Database m_db;
			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;
			RouterContext m_ctx;
			JobQueue m_jobQueue;
			JobRunnerPool m_jobRunnerPool;
			TransportPtr m_transport;
	};
}

#endif
