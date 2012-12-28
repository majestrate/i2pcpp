#include "Router.h"

#include "ssu/UDPTransport.h"

namespace i2pcpp {
	void Router::start()
	{
		m_transport.begin(Endpoint("127.0.0.1", 27333)); // TODO Pull from DB
	}

	void Router::stop()
	{
		m_transport.shutdown();
	}
}
