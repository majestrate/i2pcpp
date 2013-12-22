#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include <unordered_map>
#include <mutex>

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"
#include "transport/Transport.h"
#include "dht/Kademlia.h"

#include "Log.h"

namespace i2pcpp {
    class RouterContext;

    class OutboundMessageDispatcher {
        public:
            typedef std::unordered_multimap<RouterHash, I2NP::MessagePtr> MapType;

            OutboundMessageDispatcher(RouterContext &ctx);
            OutboundMessageDispatcher(const OutboundMessageDispatcher &) = delete;
            OutboundMessageDispatcher& operator=(OutboundMessageDispatcher &) = delete;

            void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg);
            void queueMessage(RouterHash const &to, I2NP::MessagePtr const &msg);
            void registerTransport(TransportPtr const &t);
            TransportPtr getTransport() const;
            void connected(RouterHash const rh);

            void dhtSuccess(DHT::Kademlia::key_type const k, DHT::Kademlia::value_type const v);
            void dhtFailure(DHT::Kademlia::key_type const k);

        private:
            RouterContext& m_ctx;
            TransportPtr m_transport;

            MapType m_pending;

            mutable std::mutex m_mutex;

            i2p_logger_mt m_log;
    };
}

#endif
