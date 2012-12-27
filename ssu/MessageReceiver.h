#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

#include "InboundMessageState.h"

#include "../util/LockingQueue.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;
		typedef LockingQueue<InboundMessageStatePtr> MessageQueue;

		class MessageReceiver {
			public:
				MessageReceiver(UDPTransport &transport) : m_transport(transport) {}

				void run();
				void addMessage(InboundMessageStatePtr const &ims);
				void notify();

			private:
				UDPTransport& m_transport;

				MessageQueue m_queue;
		};
	}
}

#endif
