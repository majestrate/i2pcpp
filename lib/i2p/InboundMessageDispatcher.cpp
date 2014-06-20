/**
 * @file InboundMessageDispatcher.cpp
 * @brief Implements InboundMessageDispatcher.cpp.
 */
#include "InboundMessageDispatcher.h"

#include "RouterContext.h"

#include "i2np/DeliveryStatus.h"
#include "i2np/DatabaseStore.h"

#include <i2pcpp/util/gzip.h>
#include <i2pcpp/datatypes/RouterInfo.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <botan/auto_rng.h>
#include <botan/pipe.h>

#include <iomanip>

namespace i2pcpp {
    InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx) :
        m_ios(ios),
        m_ctx(ctx),
        m_deliveryStatusHandler(ctx),
        m_dbStoreHandler(ctx),
        m_dbSearchReplyHandler(ctx),
        m_variableTunnelBuildHandler(ctx),
        m_variableTunnelBuildReplyHandler(ctx),
        m_tunnelDataHandler(ctx),
        m_tunnelGatewayHandler(ctx),
        m_log(I2P_LOG_CHANNEL("IMD")) {}


    void InboundMessageDispatcher::messageReceived(RouterHash const from, uint32_t const msgId, ByteArray data)
    {
        I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);

        I2P_LOG(m_log, debug) << "received data: " << data;

        I2NP::MessagePtr m;
        if(msgId)
            m = I2NP::Message::fromBytes(msgId, data, false);
        else
            m = I2NP::Message::fromBytes(0, data);

        if(m) {
            I2P_LOG(m_log, info) << boost::log::add_value("i2np_ib", m->getTypeString());

            switch(m->getType())
            {
                case I2NP::Message::Type::DELIVERY_STATUS:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_deliveryStatusHandler, from, m));
                    break;

                case I2NP::Message::Type::DB_STORE:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbStoreHandler, from, m));
                    break;

                case I2NP::Message::Type::DB_SEARCH_REPLY:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbSearchReplyHandler, from, m));
                    break;

                case I2NP::Message::Type::VARIABLE_TUNNEL_BUILD:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_variableTunnelBuildHandler, from, m));
                    break;

                case I2NP::Message::Type::VARIABLE_TUNNEL_BUILD_REPLY:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_variableTunnelBuildReplyHandler, from, m));
                    break;

                case I2NP::Message::Type::TUNNEL_DATA:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_tunnelDataHandler, from, m));
                    break;

                case I2NP::Message::Type::TUNNEL_GATEWAY:
                    m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_tunnelGatewayHandler, from, m));
                    break;

                case I2NP::Message::Type::GARLIC:
                    break;

                default:
                    I2P_LOG(m_log, error) << "dropping unhandled message of type " << (int)m->getType();
                    break;
            }           
        }
    }

    void InboundMessageDispatcher::connectionEstablished(RouterHash const rh, bool inbound)
    {
        I2P_LOG_SCOPED_TAG(m_log, "RouterHash", rh);
        I2P_LOG(m_log, info) << "session established";

        if(inbound) {
            Botan::AutoSeeded_RNG rng;
            uint32_t msgId;
            rng.randomize((unsigned char *)&msgId, sizeof(msgId));

            I2NP::MessagePtr m(new I2NP::DeliveryStatus(msgId, Date(2)));
            m_ctx.getOutMsgDisp().sendMessage(rh, m);

            // TODO Get this out of here
            Mapping am;
            am.setValue("caps", "BC");
            am.setValue("host", m_ctx.getDatabase()->getConfigValue("ssu_external_ip"));
            am.setValue("key", Base64::encode(m_ctx.getIdentity()->getHash()));
            am.setValue("port", m_ctx.getDatabase()->getConfigValue("ssu_external_port"));
            RouterAddress a(5, Date(0), "SSU", am);

            Mapping rm;
            rm.setValue("coreVersion", "0.9.11");
            rm.setValue("netId", "2");
            rm.setValue("router.version", "0.9.11");
            rm.setValue("stat_uptime", "90m");
            rm.setValue("caps", "OR");
            RouterInfo myInfo(*m_ctx.getIdentity(), Date(), rm);
            myInfo.addAddress(a);
            myInfo.sign(m_ctx.getSigningKey());

            Botan::Pipe gzPipe(new Gzip_Compression);
            gzPipe.start_msg();
            gzPipe.write(myInfo.serialize());
            gzPipe.end_msg();

            unsigned int size = gzPipe.remaining();
            ByteArray gzInfoBytes(size);
            gzPipe.read(gzInfoBytes.data(), size);

            auto mydsm = std::make_shared<I2NP::DatabaseStore>(myInfo.getIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes);
            m_ctx.getOutMsgDisp().sendMessage(rh, mydsm);
        }

        m_ctx.getSignals().invokePeerConnected(rh);
    }

    void InboundMessageDispatcher::connectionFailure(RouterHash const rh)
    {
        m_ctx.getSignals().invokeConnectionFailure(rh);
    }
}
