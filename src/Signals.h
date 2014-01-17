/**
 * @file Signals.h
 * @brief Defines the i2pcpp::Signals class.
 */
#ifndef SIGNALS_H
#define SIGNALS_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "datatypes/BuildRecord.h"
#include "datatypes/RouterHash.h"

namespace i2pcpp {
    class Signals {
        public:
            /**
             * Signal invoked upon storage in the database.
             */
            typedef boost::signals2::signal<void(const RouterHash, const StaticByteArray<32>, bool)> DatabaseStore;

            /**
             * Signal invoked upon receival of a tunnel build request.
             */
            typedef boost::signals2::signal<void(const uint32_t, std::list<BuildRecordPtr>)> BuildTunnelRequest;

            /**
             * Signal invoked upon connection of a peer.
             */
            typedef boost::signals2::signal<void(const RouterHash)> PeerConnected;

            /**
             * Signal invoked upon failure of connection.
             */
            typedef boost::signals2::signal<void(const RouterHash)> ConnectionFailure;

            /**
             * Signal invoked upon response to a DHT search operation.
             */
            typedef boost::signals2::signal<void(const RouterHash, const StaticByteArray<32>, const std::list<RouterHash>)> SearchReply;

            /**
             * Signal invoked upon receival of tunnel gateway data.
             */
            typedef boost::signals2::signal<void(const RouterHash, const uint32_t, const ByteArray)> TunnelGatewayData;

            /**
             * Signal invoked upon receival of tunnel data.
             */
            typedef boost::signals2::signal<void(const RouterHash, const uint32_t, const StaticByteArray<1024>)> TunnelData;

            /**
             * Constructs from a reference to an I/O service.
             */
            Signals(boost::asio::io_service &ios) : m_ios(ios) {}
            Signals(const Signals &) = delete;
            Signals& operator=(Signals &) = delete;

            /**
             * Invokes the database store event.
             * @param from the RH of the sending router
             * @param k the AES-256 key.
             * @param isRouterInfo true if we are storing RI, false otherwise
             */
            void invokeDatabaseStore(RouterHash const &from, StaticByteArray<32> const &k, bool isRouterInfo = true);

            /**
             * Registers an i2pcpp::Signals::DatabaseStore signal handler.
             */
            boost::signals2::connection registerDatabaseStore(DatabaseStore::slot_type const &dbsh);

            /**
             * Invokes the tunnel records received event.
             * @param msgId the message identifier of the original outbound message
             * @param records a list of pointers to i2pcpp::BuildRecord objects
             */
            void invokeTunnelRecordsReceived(uint32_t const msgId, std::list<BuildRecordPtr> const &records);

            /**
             * Registers an i2pcpp::Signals::TunnelRecordsReceived signal handler.
             */
            boost::signals2::connection registerTunnelRecordsReceived(BuildTunnelRequest::slot_type const &btrh);

            /**
             * Invokes the peer connected signal.
             * @param rh the i2pcpp::RouterHash of the peer.
             */
            void invokePeerConnected(RouterHash const &rh);

            /**
             * Registers an i2pcpp::Signals::PeerConnected signal handler.
             */
            boost::signals2::connection registerPeerConnected(PeerConnected::slot_type const &pch);

            /**
             * Invokes the connection failure signal.
             * @param rh the i2pcpp::RouterHash of the peer we failed to connect to
             */
            void invokeConnectionFailure(RouterHash const &rh);

            /**
             * Registers an i2pcpp::Signals::ConnectionFailure signal handler.
             */
            boost::signals2::connection registerConnectionFailure(ConnectionFailure::slot_type const &cfh);

            /**
             * Invokes the DHT search reply signal.
             * @param from the i2pcpp::RouterHash of the sending peer
             * @param query the 32 byte query
             * @param hashes a list of i2pcpp::RouterHashes returned by the search
             */
            void invokeSearchReply(RouterHash const &from, StaticByteArray<32> const &query, std::list<RouterHash> const &hashes);

            /**
             * Registers an i2pcpp::Signals::SearchReply signal handler.
             */
            boost::signals2::connection registerSearchReply(SearchReply::slot_type const &srh);

            /**
             * Invokes the tunnel gateway data signal.
             * @param from the i2pcpp::RouterHash of the router that sent the data
             * @param tunnelId the ID of the associated tunnel
             * @param data the received data
             */
            void invokeTunnelGatewayData(RouterHash const &from, uint32_t const tunnelId, ByteArray const &data);

            /**
             * Registers an i2pcpp::Signals::TunnelGatewayData signal handler.
             */
            boost::signals2::connection registerTunnelGatewayData(TunnelGatewayData::slot_type const &tgdh);

            /**
             * Invokes the tunnel data signal.
             * @param from the i2pcpp::RouterHash of the router that sent the data
             * @param tunnelId the ID of the associated tunnel
             * @param data the 1024 bytes of received data
             */
            void invokeTunnelData(RouterHash const &from, uint32_t const tunnelId, StaticByteArray<1024> const &data);

            /**
             * Registers an i2pcpp::Signals::TunnelData signal handler.
             */
            boost::signals2::connection registerTunnelData(TunnelData::slot_type const &tdh);

        private:
            boost::asio::io_service& m_ios;

            DatabaseStore m_databaseStore;
            BuildTunnelRequest m_buildTunnelRequest;
            PeerConnected m_peerConnected;
            ConnectionFailure m_connectionFailure;
            SearchReply m_searchReply;
            TunnelGatewayData m_tunnelGatewayData;
            TunnelData m_tunnelData;
    };
}

#endif
