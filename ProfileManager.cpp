#include "ProfileManager.h"

#include "RouterContext.h"

namespace i2pcpp {
	ProfileManager::ProfileManager(RouterContext &ctx) :
		m_ctx(ctx) {}

	RouterInfo ProfileManager::getPeer()
	{
		return m_ctx.getDatabase().getRouterInfo(m_ctx.getDatabase().getRandomRouter());
	}

	void ProfileManager::expungePeer(RouterHash const & rh) {}
}
