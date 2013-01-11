#ifndef TUNNEL_H
#define TUNNEL_H

#include <list>
#include <vector>

#include "../RouterContext.h"
#include "../datatypes/RouterHash.h"
#include "../datatypes/BuildRequestRecord.h"

namespace i2pcpp {
	class Tunnel {
		public:
			Tunnel(RouterContext &ctx, std::list<RouterHash> const &hops);

		private:
			RouterContext& m_ctx;

			std::vector<BuildRequestRecord> m_brrs;
	};
}

#endif
