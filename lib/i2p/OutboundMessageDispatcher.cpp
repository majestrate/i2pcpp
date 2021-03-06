/**
 * @file OutboundMessageDispatcher.cpp
 * @brief Implements OutboundMessageDispatcher.h
 */
#include "OutboundMessageDispatcher.h"

#include "RouterContext.h"

#include "i2np/Message.h"

#include <i2pcpp/datatypes/RouterInfo.h>

namespace i2pcpp {
    OutboundMessageDispatcher::OutboundMessageDispatcher(RouterContext &ctx) :
        m_ctx(ctx),
        m_log(I2P_LOG_CHANNEL("OMD")) {}

    void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
    {
        if(!m_transport) throw std::logic_error("No transport registered");

        if(to == m_ctx.getIdentity()->getHash()) {
            I2P_LOG(m_log, debug) << "message is for myself, sending to IMD";
            m_ctx.getInMsgDisp().messageReceived(to, msg->getMsgId(), msg->toBytes(false));
            return;
        }

        if(m_transport->isConnected(to)) {
            I2P_LOG(m_log, info) << boost::log::add_value("i2np_ob", (std::string) msg->getTypeString());
            // SSU is the only transport implemented, so use the short header
            m_transport->send(to, msg->getMsgId(), msg->toBytes(false));
        } else {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", to);
            I2P_LOG(m_log, debug) << "not connected, queueing message";

            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending.insert(MapType::value_type(to, msg));

            if(m_ctx.getDatabase()->routerExists(to))
                m_transport->connect(m_ctx.getDatabase()->getRouterInfo(to));
            else {
                I2P_LOG(m_log, debug) << "RouterInfo not in DB, creating search job";
                bool result = m_ctx.getDHT()->lookup(to);
                if(!result) {
                    I2P_LOG(m_log, error) << "could not find a good place to start search, aborting";
                    m_pending.erase(to);
                }
            }
        }
    }

    void OutboundMessageDispatcher::registerTransport(TransportPtr const &t)
    {
        m_transport = t;
    }

    TransportPtr OutboundMessageDispatcher::getTransport() const
    {
        return m_transport;
    }

    void OutboundMessageDispatcher::connected(RouterHash const rh)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto bucket = m_pending.equal_range(rh);
        for(auto itr = bucket.first; itr != bucket.second; ++itr) {
            I2P_LOG(m_log, debug) << "connected to peer, flushing queue";

            // Short header, as above
            m_transport->send(itr->first, itr->second->getMsgId(), itr->second->toBytes(false));
        }
        // we have succeeded and flushed data, we are no longer pending
        m_pending.erase(rh);
    }

    void OutboundMessageDispatcher::dhtSuccess(DHT::Kademlia::key_type const k, DHT::Kademlia::value_type const v)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(m_pending.count(v)) {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", v);
            I2P_LOG(m_log, debug) << "DHT lookup succeeded, connecting to peer";

            m_transport->connect(m_ctx.getDatabase()->getRouterInfo(v));
        }
    }

    void OutboundMessageDispatcher::dhtFailure(DHT::Kademlia::key_type const k)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(m_pending.count(k)) {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", k);
            I2P_LOG(m_log, debug) << "DHT lookup failed, tossing queued messages";

            m_pending.erase(k);
        }
    }
}
