#include "InboundMessageDispatcher.h"

#include <boost/bind.hpp>

namespace i2pcpp {
	InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios) :
		m_ios(ios),
 		m_log(boost::log::keywords::channel = "IMD") {}

	void InboundMessageDispatcher::messageReceived(RouterHash from, ByteArray data)
	{
		I2P_LOG_RH(m_log, from);

		std::stringstream s;
		s << std::setw(2) << std::setfill('0') << std::hex;
		for(auto c: data) s << (int)c;
		BOOST_LOG_SEV(m_log, debug) << "received data: " << s.str();
	}

	void InboundMessageDispatcher::connectionEstablished(RouterHash rh)
	{
		I2P_LOG_RH(m_log, rh);
		BOOST_LOG_SEV(m_log, info) << "session established";
	}
}
