#ifndef SSUMESSAGERECEIVER_H
#define SSUMESSAGERECEIVER_H

#include "InboundMessageState.h"

#include "../Thread.h"

#include "../util/LockingQueue.h"

namespace i2pcpp {
	class RouterContext;

	namespace SSU {
		class MessageReceiver : public Thread {
			public:
				MessageReceiver(RouterContext &ctx) : m_ctx(ctx) {}

				void addMessage(InboundMessageStatePtr const &ims);

			private:
				void loop();
				void stopHook() { m_queue.finish(); }

				RouterContext& m_ctx;

				LockingQueue<InboundMessageStatePtr> m_queue;
		};
	}
}

#endif
