#include "ChiPeerManager.h"
#include "RouterContext.h"


namespace i2pcpp {

	void ChiPeerManager::connected(RouterHash rh)
	{
	}

	void ChiPeerManager::failure(RouterHash rh)
	{

		std::string rhstr = rh;

		if ( m_stats.count(rhstr) == 0 ) {
			auto stat = &m_stats[rh];
			stat->failed = 1;
			stat->connect = 0;
			stat->disconnect = 0;
		} else {
			m_stats[rh].failed ++;
		}

		if ( m_stats[rh].shouldRemove() ) {
			m_ctx.getProfileManager().expungePeer(rh);
			m_stats.erase(rh);
		}
	}
	
	void ChiPeerManager::disconnected(RouterHash rh)
	{
	}

}
