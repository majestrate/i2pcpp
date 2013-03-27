#include "OutboundMessageDispatcher.h"

namespace i2pcpp {
	void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
	{
		// TODO Exception if otherwise
		if(m_transport)
			m_transport->send(to, msg->toBytes());
	}

	void OutboundMessageDispatcher::registerTransport(TransportPtr const &t)
	{
		/* This should actually register the transport.
		 * Trying to keep things simple for now. */
		m_transport = t;
	}

	TransportPtr OutboundMessageDispatcher::getTransport() const
	{
		return m_transport;
	}
}
