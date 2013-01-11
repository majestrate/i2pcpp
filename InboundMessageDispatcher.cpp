#include "InboundMessageDispatcher.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace i2pcpp {
	void InboundMessageDispatcher::receiveMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
	{
		I2NP::Message::Type mtype = msg->getType();
		if(m_msgHandlers.count(mtype) > 0)
			get_io_service().post(std::bind(&Handlers::Message::handleMessage, m_msgHandlers[mtype], from, msg));
		else
			std::cerr << "InboundMessageDispatcher: dropping packet of type " << msg->getType() << " without a handler\n";
	}

	void InboundMessageDispatcher::registerHandler(I2NP::Message::Type const mtype, Handlers::MessagePtr const &handler)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_msgHandlers[mtype] = handler;
	}

	void InboundMessageDispatcher::unregisterHandler(I2NP::Message::Type const mtype)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_msgHandlers.erase(mtype);
	}
}
