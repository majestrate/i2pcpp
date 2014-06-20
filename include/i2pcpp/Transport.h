/*
 * @file Transport.h
 * @brief Implements the i2pcpp::Transport base class.
 */
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <i2pcpp/datatypes/RouterHash.h>

#include <boost/signals2.hpp>

namespace i2pcpp {
    class RouterInfo;

    /**
     * Provides an abstract base class for different transport types.
     */
    class Transport {
        public:
            typedef boost::signals2::signal<void(const RouterHash, bool)> EstablishedSignal;
            typedef boost::signals2::signal<void(const RouterHash, const uint32_t, const ByteArray)> ReceivedSignal;
            typedef boost::signals2::signal<void(const RouterHash)> FailureSignal;
            typedef boost::signals2::signal<void(const RouterHash)> DisconnectedSignal;

            Transport() = default;
            Transport(const Transport &) = delete;
            Transport& operator=(Transport &) = delete;

            virtual ~Transport();

            /**
             * Connects to a router given by its i2pcpp::RouterInfo structure.
             */
            virtual void connect(RouterInfo const &ri) = 0;

            /**
             * Sends a message (i2pcpp::ByteArray) to a peer
             *  (given by an i2pcpp::RouterHash).
             * @param rh the i2pcpp::RouterHash that identifies the peer
             * @param msg the data to be send
             */
            virtual void send(RouterHash const &rh, uint32_t msgId, ByteArray const &msg) = 0;

            /**
             * Closes the connection with the peer (given by its i2pcpp::RouterHash \a rh).
             */
            virtual void disconnect(RouterHash const &rh) = 0;

            /**
             * @return the number of peers connected to
             */
            virtual uint32_t numPeers() const = 0;

            /**
             * @return true if there is a connection with the peer given by its
             *   i2pcpp::RouterHash, false otherwise.
             */
            virtual bool isConnected(RouterHash const &rh) const = 0;

            /**
             * Registers the i2pcpp::EstablishedSignal.
             */
            boost::signals2::connection registerEstablishedHandler(EstablishedSignal::slot_type const &eh);

            /**
             * Registers the i2pcpp::ReceivedSignal.
             */
            boost::signals2::connection registerReceivedHandler(ReceivedSignal::slot_type const &rh);

            /**
             * Registers the i2pcpp::FailureSignal.
             */
            boost::signals2::connection registerFailureSignal(FailureSignal::slot_type const &fs);

            /**
             * Registers the i2pcpp::DisconnectedSignal.
             */
            boost::signals2::connection registerDisconnectedSignal(DisconnectedSignal::slot_type const &ds);

            /**
             * stop accepting new peers
             */
            virtual void gracefulShutdown() {}

        protected:
            EstablishedSignal m_establishedSignal;
            ReceivedSignal m_receivedSignal;
            FailureSignal m_failureSignal;
            DisconnectedSignal m_disconnectedSignal;
    };

    typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
