#include "Router.h"

/* TEMPORARY */
#include "util/Base64.h"
#include "transport/UDPTransport.h"

namespace i2pcpp {
	Router::Router(std::string const &dbFile) :
		m_work(m_ios) {}

	Router::~Router()
	{
		if(m_serviceThread.joinable()) m_serviceThread.join();
	}

	void Router::start()
	{
		m_serviceThread = std::thread([&](){m_ios.run();});

		/*m_transport = TransportPtr(new UDPTransport());

		std::shared_ptr<UDPTransport> u = std::dynamic_pointer_cast<UDPTransport>(m_transport);
		u->start(Endpoint("127.0.0.1", 12345));*/
	}

	void Router::stop()
	{
		m_ios.stop();
	}
}
