#ifndef TUNNEL_PROFILER_H
#define TUNNEL_PROFILER_H

#include "../datatypes/RouterIdentity.h"
#include "../Database.h"

#include <vector>

namespace i2pcpp {

	class TunnelGenerator {
	public:
		TunnelGenerator(Database & db);
		~TunnelGenerator();
		
		std::vector<RouterIdentity> makeTunnelHops(uint8_t hops);
		
	private:
		Database & m_db;

	};

}

#endif
