#include "Signals.h"

namespace i2pcpp {
	void Signals::invokeDatabaseStore(RouterHash const &from, std::array<unsigned char, 32> const &k, bool isRouterInfo)
	{
		m_ios.post(boost::bind(boost::ref(m_databaseStore), from, k, isRouterInfo));
	}

	boost::signals2::connection Signals::registerDatabaseStore(DatabaseStore::slot_type const &dbsh)
	{
		return m_databaseStore.connect(dbsh);
	}

	void Signals::invokeTunnelRecordsReceived(const uint32_t msgId, const std::list<BuildRecordPtr> &records)
	{
		m_ios.post(boost::bind(boost::ref(m_buildTunnelRequest), msgId, records));
	}

	boost::signals2::connection Signals::registerTunnelRecordsReceived(BuildTunnelRequest::slot_type const &btrh)
	{
		return m_buildTunnelRequest.connect(btrh);
	}

	void Signals::invokePeerConnected(RouterHash const &rh)
	{
		m_ios.post(boost::bind(boost::ref(m_peerConnected), rh));
	}

	boost::signals2::connection Signals::registerPeerConnected(PeerConnected::slot_type const &pch)
	{
		return m_peerConnected.connect(pch);
	}

	void Signals::invokeConnectionFailure(RouterHash const &rh)
	{
		m_ios.post(boost::bind(boost::ref(m_connectionFailure), rh));
	}

	boost::signals2::connection Signals::registerConnectionFailure(ConnectionFailure::slot_type const &cfh)
	{
		return m_connectionFailure.connect(cfh);
	}

	void Signals::invokeSearchReply(RouterHash const &from, std::array<unsigned char, 32> const &query, std::list<RouterHash> const &hashes)
	{
		m_ios.post(boost::bind(boost::ref(m_searchReply), from, query, hashes));
	}

	boost::signals2::connection Signals::registerSearchReply(SearchReply::slot_type const &srh)
	{
		return m_searchReply.connect(srh);
	}

	void Signals::invokeTunnelGatewayData(const uint32_t tunnelId, ByteArray const &data)
	{
		m_ios.post(boost::bind(boost::ref(m_tunnelGatewayData), tunnelId, data));
	}

	boost::signals2::connection Signals::registerTunnelGatewayData(TunnelGatewayData::slot_type const &tgdh)
	{
		return m_tunnelGatewayData.connect(tgdh);
	}

	void Signals::invokeTunnelData(uint32_t const tunnelId, std::array<unsigned char, 1024> const &data)
	{
		m_ios.post(boost::bind(boost::ref(m_tunnelData), tunnelId, data));
	}

	boost::signals2::connection Signals::registerTunnelData(TunnelData::slot_type const &tdh)
	{
		return m_tunnelData.connect(tdh);
	}
}
