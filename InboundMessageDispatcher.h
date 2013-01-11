#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <boost/asio.hpp>

#include "handlers/DatabaseStore.h"
#include "handlers/DatabaseSearchReply.h"

#include "RouterContext.h"

namespace i2pcpp {
	class InboundMessageDispatcher {
		public:
			InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx);

			void messageReceived(const RouterHash &from, const ByteArray &data);
			void connectionEstablished(const RouterHash &rh);

		private:
			boost::asio::io_service& m_ios;
			RouterContext& m_ctx;

			Handlers::DatabaseStore m_dbStoreHandler;
			Handlers::DatabaseSearchReply m_dbSearchReplyHandler;
	};
}

#endif
