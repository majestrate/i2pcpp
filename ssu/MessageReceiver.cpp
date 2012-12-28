#include "MessageReceiver.h"

#include "PacketHandler.h"
#include "UDPTransport.h"

#include "../i2np/Message.h"

namespace i2pcpp {
	namespace SSU {
		void MessageReceiver::loop()
		{
			InboundMessageDispatcher &imd = m_transport.getInMsgDispatcher();

			while(m_keepRunning)
			{
				m_queue.wait();

				InboundMessageStatePtr ims = m_queue.pop();

				if(!ims)
					continue;

				std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: Received IMS with " << (int)ims->getNumFragments() << " fragments\n";

				ByteArray data;
				ims->assemble(data);
				I2NP::MessagePtr m = I2NP::Message::fromBytes(data);

				if(m) {
					std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: This looks like a message of type: " << (int)m->getType() << "\n";

					imd.addMessage(m);
				}
			}
		}

		void MessageReceiver::addMessage(InboundMessageStatePtr const &ims)
		{
			m_queue.enqueue(ims);
		}
	}
}
