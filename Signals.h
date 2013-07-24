#ifndef SIGNALS_H
#define SIGNALS_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "datatypes/BuildRecord.h"
#include "datatypes/RouterHash.h"

namespace i2pcpp {
	class Signals {
		public:
			typedef boost::signals2::signal<void(const RouterHash, const std::array<unsigned char, 32>, bool)> DatabaseStore;
			typedef boost::signals2::signal<void(std::list<BuildRecordPtr>)> BuildTunnelRequest;
			typedef boost::signals2::signal<void(const RouterHash)> PeerConnected;
			typedef boost::signals2::signal<void(const RouterHash)> ConnectionFailure;
			typedef boost::signals2::signal<void(const RouterHash, const std::array<unsigned char, 32>, const std::list<RouterHash>)> SearchReply;
			typedef boost::signals2::signal<void(const uint32_t, const ByteArray)> TunnelGatewayData;

			Signals(boost::asio::io_service &ios) : m_ios(ios) {}

			void invokeDatabaseStore(RouterHash const &from, std::array<unsigned char, 32> const &k, bool isRouterInfo = true);
			boost::signals2::connection registerDatabaseStore(DatabaseStore::slot_type const &dbsh);

			void invokeTunnelRecordsReceived(std::list<BuildRecordPtr> const &records);
			boost::signals2::connection registerTunnelRecordsReceived(BuildTunnelRequest::slot_type const &btrh);

			void invokePeerConnected(RouterHash const &rh);
			boost::signals2::connection registerPeerConnected(PeerConnected::slot_type const &pch);

			void invokeConnectionFailure(RouterHash const &rh);
			boost::signals2::connection registerConnectionFailure(ConnectionFailure::slot_type const &cfh);

			void invokeSearchReply(RouterHash const &from, std::array<unsigned char, 32> const &query, std::list<RouterHash> const &hashes);
			boost::signals2::connection registerSearchReply(SearchReply::slot_type const &srh);

			void invokeTunnelGatewayData(const uint32_t tunnelId, ByteArray const &data);
			boost::signals2::connection registerTunnelGatewayData(TunnelGatewayData::slot_type const &tgdh);

		private:
			boost::asio::io_service& m_ios;

			DatabaseStore m_databaseStore;
			BuildTunnelRequest m_buildTunnelRequest;
			PeerConnected m_peerConnected;
			ConnectionFailure m_connectionFailure;
			SearchReply m_searchReply;
			TunnelGatewayData m_tunnelGatewayData;
	};
}

#endif
