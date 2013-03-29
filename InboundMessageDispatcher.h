#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "handlers/DeliveryStatus.h"
#include "handlers/DatabaseStore.h"
#include "handlers/DatabaseSearchReply.h"
#include "handlers/VariableTunnelBuild.h"

#include "Log.h"

namespace i2pcpp {
	class InboundMessageDispatcher {
		public:
			InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx);

			void messageReceived(RouterHash from, ByteArray data);
			void connectionEstablished(RouterHash rh, bool inbound);

		private:
			boost::asio::io_service& m_ios;
			RouterContext& m_ctx;

			Handlers::DeliveryStatus m_deliveryStatusHandler;
			Handlers::DatabaseStore m_dbStoreHandler;
			Handlers::DatabaseSearchReply m_dbSearchReplyHandler;
			Handlers::VariableTunnelBuild m_variableTunnelBuildHandler;

			i2p_logger_mt m_log;
	};
}

#endif
