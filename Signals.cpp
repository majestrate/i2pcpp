#include "Signals.h"

namespace i2pcpp {
	void Signals::invokeRouterInfoSaved(const RouterHash &rh)
	{
		m_ios.post(boost::bind(boost::ref(m_routerInfoSaved), rh));
	}

	boost::signals2::connection Signals::registerRouterInfoSaved(RouterInfoSaved::slot_type const &rish)
	{
		return m_routerInfoSaved.connect(rish);
	}

	void Signals::invokeTunnelRecordsReceived(const std::list<BuildRecord> &records)
	{
		m_ios.post(boost::bind(boost::ref(m_buildTunnelRequest), records));
	}

	boost::signals2::connection Signals::registerTunnelRecordsReceived(BuildTunnelRequest::slot_type const &btrh)
	{
		return m_buildTunnelRequest.connect(btrh);
	}
}
