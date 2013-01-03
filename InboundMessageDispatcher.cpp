#include "InboundMessageDispatcher.h"

#include <iostream>

namespace i2pcpp {
	void InboundMessageDispatcher::receiveMessage(RouterHash const &from, I2NP::MessagePtr const &msg) const
	{
		I2NP::Message::Type mtype = msg->getType();
		if(m_msgHandlers.count(mtype) > 0) {
			JobPtr j = (m_msgHandlers.find(mtype))->second->createJob(from, msg);
			if(j)
				m_jobQueue.enqueue(j);
		}	else
			std::cerr << "InboundMessageDispatcher: Dropping packet of type " << msg->getType() << " without a handler\n";
	}

	void InboundMessageDispatcher::registerHandler(I2NP::Message::Type const mtype, MessageHandlerPtr const &handler)
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
