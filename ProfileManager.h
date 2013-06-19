#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "datatypes/RouterInfo.h"

namespace i2pcpp {

	class RouterContext;

	class ProfileManager {
		public:
			ProfileManager(RouterContext &ctx);
			ProfileManager(const ProfileManager &) = delete;
			ProfileManager& operator=(ProfileManager &) = delete;

			virtual RouterInfo getPeer();

		private:
			RouterContext& m_ctx;
	};
}

#endif
