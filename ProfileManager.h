#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "datatypes/RouterInfo.h"

namespace i2pcpp {
	class RouterContext;

	class ProfileManager {
		public:
			ProfileManager(RouterContext &ctx);
			const RouterInfo getPeer();

		private:
			RouterContext& m_ctx;
	};
}

#endif
