#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

#include "InboundMessageState.h"

#include "../Thread.h"

#include "../util/LockingQueue.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;
		typedef LockingQueue<InboundMessageStatePtr> MessageQueue;

		class MessageReceiver : public Thread {
			public:
				MessageReceiver(UDPTransport &transport) : m_transport(transport) {}

				void addMessage(InboundMessageStatePtr const &ims);

			private:
				void loop();
				void stopHook() { m_queue.finish(); }

				UDPTransport& m_transport;

				MessageQueue m_queue;
		};
	}
}

#endif
