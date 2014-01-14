/**
 * @file InboundMessageDispatcher.h
 * @brief Defines the i2pcpp::InboundMessageDispatcher class.
 */
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
    /**
     * Dispatches inbound messages to the correct i2pcpp::Handlers.
     */
    class InboundMessageDispatcher {
        public:
            /**
             * Constructs from a reference to an I/O service and an
             *  i2pcpp::RouterContext.
             */
            InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx);
            InboundMessageDispatcher(const InboundMessageDispatcher &) = delete;
            InboundMessageDispatcher& operator=(InboundMessageDispatcher &) = delete;

            /**
             * Called whenever an i2pcpp::Transport receives a message.
             * @param from the i2pcpp::RouterHash of the sending router
             * @param msgId the ID of the original outbound message
             * @param data the actual received data
             */
            void messageReceived(RouterHash const from, uint32_t const msgId, ByteArray data);

            /**
             * Called when a connection with a router has been established.
             * @param rh the i2pcpp::RouterHash of the router we are now connected with
             * @param inbound true if the connection is inbound, false otherwise
             */
            void connectionEstablished(RouterHash const rh, bool inbound);

            /**
             * Called when connection establishment with a router has failed.
             * @param rh the i2pcpp::RouterHash of the router we attempted to
             *  connect with
             */
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
