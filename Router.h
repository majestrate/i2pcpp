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
#include "JobScheduler.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile) :
				m_db(dbFile),
				m_jobScheduler(m_jobQueue),
				m_inMsgDispatcher(m_jobQueue),
				m_ctx(m_db, m_inMsgDispatcher, m_outMsgDispatcher) {}

			void start();
			void stop();

			/* These are only temporary and will by used for
			 * testing and debugging. */

			void databaseLookup(std::string const &to, std::string const &query);

		private:
			Database m_db;
			JobQueue m_jobQueue;
			JobScheduler m_jobScheduler;
			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;
			RouterContext m_ctx;
			JobRunnerPool m_jobRunnerPool;
			TransportPtr m_transport;
	};
}

#endif
