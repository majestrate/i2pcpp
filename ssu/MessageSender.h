#ifndef SSUMESSAGESENDER_H
#define SSUMESSAGESENDER_H

#include "OutboundMessageState.h"

#include "../Thread.h"

#include "../util/LockingQueue.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;
		typedef LockingQueue<OutboundMessageStatePtr> OutboundMessageQueue;

		class MessageSender : public Thread {
			public:
				MessageSender(UDPTransport &transport) : m_transport(transport) {}

				void addMessage(OutboundMessageStatePtr const &oms);

			private:
				void loop();
				void stopHook() { m_queue.finish(); }

				UDPTransport& m_transport;

				OutboundMessageQueue m_queue;
		};
	}
}

#endif
