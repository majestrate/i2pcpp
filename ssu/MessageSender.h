#ifndef SSUMESSAGESENDER_H
#define SSUMESSAGESENDER_H

#include "../util/LockingQueue.h"

#include "../Thread.h"

#include "OutboundMessageState.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class MessageSender : public Thread {
			public:
				MessageSender(UDPTransport &transport) : m_transport(transport) {}

				void addWork(PeerStatePtr const &ps) { m_queue.enqueue(ps); }

			private:
				void loop();
				void stopHook() { m_queue.finish(); }

				UDPTransport& m_transport;

				LockingQueue<PeerStatePtr> m_queue;
		};
	}
}

#endif
