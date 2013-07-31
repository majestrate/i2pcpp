#ifndef CACHING_PROFILE_MANAGER_H_
#define CACHING_PROFILE_MANAGER_H_
#include "ProfileManager.h"

#include <unordered_map>

namespace i2pcpp {

	class CachingProfileManager : public ProfileManager {

		typedef std::unordered_map<RouterHash, RouterInfo> Cache;
	
	public:
		CachingProfileManager(RouterContext &ctx);
		CachingProfileManager(const ProfileManager &) = delete;
		CachingProfileManager& operator=(ProfileManager &) = delete;
		
		RouterInfo getPeer();
		void expungePeer(RouterHash const & rh);

	private:
		int counter=0;
		Cache m_cache;

		void populate_cache(int num);

	};
}

#endif
