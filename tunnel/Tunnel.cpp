#include "Tunnel.h"

namespace i2pcpp {
	Tunnel::Tunnel(Direction d, std::vector<RouterHash> const &hops) :
		m_direction(d)
	{
		if(d == OUTBOUND) {
			for(int i = 0; i < hops.size(); i++) {
				// Make the last hop reply back to you.
				m_hops.push_back(std::make_shared<TunnelHop>(hops[i], hops[i + 1]));
				if(i == hops.size() - 1)
					m_hops.back()->setType(TunnelHop::ENDPOINT);
			}
		}
	}

	Tunnel::State Tunnel::getState() const
	{
		return m_state;
	}

	std::list<BuildRequestRecord> Tunnel::getRecords() const
	{
		std::list<BuildRequestRecord> recs;

		for(auto h: m_hops)
			recs.emplace_back(h);

		return recs;
	}
}
