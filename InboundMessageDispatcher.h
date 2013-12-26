#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "handlers/DeliveryStatus.h"
#include "handlers/DatabaseStore.h"
#include "handlers/DatabaseSearchReply.h"
#include "handlers/VariableTunnelBuild.h"
#include "handlers/VariableTunnelBuildReply.h"
#include "handlers/TunnelData.h"
#include "handlers/TunnelGateway.h"

#include "Log.h"

namespace i2pcpp {
    class InboundMessageDispatcher {
        public:
            InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx);
            InboundMessageDispatcher(const InboundMessageDispatcher &) = delete;
            InboundMessageDispatcher& operator=(InboundMessageDispatcher &) = delete;

            void messageReceived(RouterHash const from, uint32_t const msgId, ByteArray data);
            void connectionEstablished(RouterHash const rh, bool inbound);
            void connectionFailure(RouterHash const rh);

        private:
            boost::asio::io_service& m_ios;
            RouterContext& m_ctx;

            Handlers::DeliveryStatus m_deliveryStatusHandler;
            Handlers::DatabaseStore m_dbStoreHandler;
            Handlers::DatabaseSearchReply m_dbSearchReplyHandler;
            Handlers::VariableTunnelBuild m_variableTunnelBuildHandler;
            Handlers::VariableTunnelBuildReply m_variableTunnelBuildReplyHandler;
            Handlers::TunnelData m_tunnelDataHandler;
            Handlers::TunnelGateway m_tunnelGatewayHandler;

            i2p_logger_mt m_log;
    };
}

#endif
