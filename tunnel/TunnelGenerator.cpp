#include "TunnelGenerator.h"
#include "../datatypes/RouterHash.h"

namespace i2pcpp {

	TunnelGenerator::TunnelGenerator(Database & db) : m_db(db) {}
	TunnelGenerator::~TunnelGenerator() {}

	/**
		 use random peers
	 */
	std::vector<RouterIdentity> TunnelGenerator::makeTunnelHops(uint8_t hops)
	{
		std::vector<RouterIdentity> vec;
		while (hops--) {
			RouterHash rh = m_db.getRandomFloodfill();
			vec.push_back(m_db.getRouterInfo(rh).getIdentity());
		}
		return vec;
	}


}
