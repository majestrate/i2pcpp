#include "MessageSender.h"

namespace i2pcpp {
	namespace SSU {
		void MessageSender::loop()
		{
			while(m_keepRunning)
			{
				m_queue.wait();

				OutboundMessageStatePtr oms = m_queue.pop();

				if(!oms)
					continue;
			}
		}

		void MessageSender::addMessage(OutboundMessageStatePtr const &oms)
		{
			m_queue.enqueue(oms);
		}
	}
}
