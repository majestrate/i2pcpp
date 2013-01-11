#include "OutboundMessageDispatcher.h"

namespace i2pcpp {
	void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
	{
		get_io_service().post(std::bind(&Transport::send, m_transport, to, msg));
	}

	void OutboundMessageDispatcher::registerTransport(TransportPtr const &t)
	{
		/* This should actually register the transport.
		 * Trying to keep things simple for now. */
		m_transport = t;
	}
}
