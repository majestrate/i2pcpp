#include "InboundMessageDispatcher.h"

namespace i2pcpp {
	void InboundMessageDispatcher::addMessage(I2NP::MessagePtr const &msg)
	{
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
