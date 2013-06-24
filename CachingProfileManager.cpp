#include "CachingProfileManager.h"
#include "RouterContext.h"

#define FF_CACHE_SIZE 1000

namespace i2pcpp {

	CachingProfileManager::CachingProfileManager(RouterContext & rctx) : ProfileManager(rctx) {}

	RouterInfo CachingProfileManager::getPeer()
	{
		if ( m_cache.size() == 0 ) {
			populate_cache(FF_CACHE_SIZE);
		}
		auto itr = m_cache.begin();
		for (int count = counter++ % m_cache.size() ; count > 0; count--) itr++;
		return itr->second;
	}

	void CachingProfileManager::expungePeer(RouterHash const & rh)
	{
		m_cache.erase(rh);
	}

	void CachingProfileManager::populate_cache(int num)
	{
		Database & db = m_ctx.getDatabase();
		for ( auto rh : db.getRandomFloodfills(num) ) m_cache[rh] = db.getRouterInfo(rh);
	}

}
