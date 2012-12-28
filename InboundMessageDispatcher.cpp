#include "InboundMessageDispatcher.h"

namespace i2pcpp {
	void InboundMessageDispatcher::addMessage(I2NP::MessagePtr const &msg)
	{
		I2NP::Message::MessageType mtype = msg->getType();
		if(m_msgHandlers.count(mtype) > 0)
			m_jobQueue.enqueue(m_msgHandlers[mtype]->createJob());
	}

	void InboundMessageDispatcher::registerHandler(I2NP::Message::MessageType const mtype, MessageHandlerPtr const &handler)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_msgHandlers[mtype] = handler;
	}

	void InboundMessageDispatcher::unregisterHandler(I2NP::Message::MessageType const mtype)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_msgHandlers.erase(mtype);
	}
}
