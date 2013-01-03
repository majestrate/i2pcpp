#include "OutboundMessageDispatcher.h"

namespace i2pcpp {
	void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg) const
	{
		//m_transport->send(to, msg->toBytes());
	}
}
