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

				void addWork(PeerStatePtr const &ps, OutboundMessageStatePtr const &oms) { m_queue.enqueue(std::make_pair(ps, oms)); }

			private:
				void loop();
				void stopHook() { m_queue.finish(); }

				UDPTransport& m_transport;

				LockingQueue<std::pair<PeerStatePtr, OutboundMessageStatePtr>> m_queue;
		};
	}
}

#endif
