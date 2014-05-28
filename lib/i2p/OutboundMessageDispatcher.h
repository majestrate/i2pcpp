/**
 * @file OutboundMessageDispatcher.h
 * @brief Defines the i2pcpp::OutboundMessageDispatcher class.
 */
#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include "../../include/i2pcpp/Transport.h"

#include "dht/Kademlia.h"

#include <i2pcpp/Log.h>

#include <i2pcpp/datatypes/RouterHash.h>

#include <unordered_map>
#include <mutex>

namespace i2pcpp {
    namespace I2NP { class Message; typedef std::shared_ptr<Message> MessagePtr; }

    class RouterContext;

    /**
     * Dispatches outbound messages to the correct i2pcpp::Transport object.
     */
    class OutboundMessageDispatcher {
        public:
            typedef std::unordered_multimap<RouterHash, I2NP::MessagePtr> MapType;

            /**
             * Constructs from a reference to a reference to the i2pcpp::RouterContext.
             */
            OutboundMessageDispatcher(RouterContext &ctx);
            OutboundMessageDispatcher(const OutboundMessageDispatcher &) = delete;
            OutboundMessageDispatcher& operator=(OutboundMessageDispatcher &) = delete;

            /**
             * Sends (dispatches) a message to the transport object.
             * If there is currently no connection with the router given by
             *  \a to, queues the message for sending and preforms a DHT lookup.
             * @param to the i2pcpp::RouterHash of the router to send the
             *  message to
             * @param msg pointer the i2pcpp::I2NP::Message to send
             */
            void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg);

            /**
             * Registers an i2pcpp::Transport object to which we may dispatch
             *  messages.
             * @param t a pointer to the i2pcpp::Transport object to be registered
             */
            void registerTransport(TransportPtr const &t);

            /**
             * @return a pointer to the i2pcpp::Transport object we are dispatching
             *  messages to
             */
            TransportPtr getTransport() const;

            /**
             * Called when this router connected with a router given by \a rh.
             * At this point all pending (queued) messages for that router
             *  are dispatched to the i2pcpp::Transport object.
             */
            void connected(RouterHash const rh);

            /**
             * Called when the DHT lookup was succesful.
             * Tries to connect to the value that was extracted from the DHT.
             * @param k the key we did a lookup on
             * @param v the value associated with the key (the i2pcpp::RouterHash
             *   that allows finding the i2pcpp::RouterInfo structure in the
             *   i2pcpp::Database)
             */
            void dhtSuccess(DHT::Kademlia::key_type const k, DHT::Kademlia::value_type const v);

            /**
             * Called when the DHT lookup failed.
             * Removes all of the pending (queued) messages for the router we
             *  tried to find.
             * @param k the key we did a lookup on (the i2pcpp::RouterHash)
             */
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
