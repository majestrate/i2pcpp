#include "MessageReceiver.h"

#include "UDPTransport.h"

#include "../InboundMessageDispatcher.h"
#include "../i2np/Message.h"

namespace i2pcpp {
	namespace SSU {
		void MessageReceiver::loop()
		{
			try {
				const InboundMessageDispatcher &imd = m_ctx.getInMsgDispatcher();

				while(m_keepRunning)
				{
					InboundMessageStatePtr ims = m_queue.wait_and_pop();

					std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: Received IMS with " << (int)ims->getNumFragments() << " fragments\n";

					const ByteArray&& data = ims->assemble();

					if(data.size()) {
						I2NP::MessagePtr m = I2NP::Message::fromBytes(data);

						if(m) {
							std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: This looks like a message of type: " << (int)m->getType() << "\n";

							imd.receiveMessage(ims->getRouterHash(), m);
						}
					}
				}
			} catch(LockingQueueFinished) {}
		}

		void MessageReceiver::addMessage(InboundMessageStatePtr const &ims)
		{
			m_queue.enqueue(ims);
		}
	}
}
