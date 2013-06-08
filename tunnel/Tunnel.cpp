#include "Tunnel.h"

namespace i2pcpp {
	Tunnel::Tunnel(Direction d) :
		m_direction(d) {}

	Tunnel::State Tunnel::getState() const
	{
		return m_state;
	}
}
