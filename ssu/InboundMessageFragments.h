#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include <thread>

#include "PeerState.h"
#include "MessageReceiver.h"

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class InboundMessageFragments {
			public:
				InboundMessageFragments(RouterContext &ctx) : m_messageReceiver(ctx) {}

				void begin() { m_messageReceiver.start(); }
				void shutdown() { m_messageReceiver.stop(); }
				void receiveData(PeerStatePtr const &ps, ByteArray::const_iterator &dataItr);

			private:
				// TODO: Decaying bloom filter
				MessageReceiver m_messageReceiver;
		};
	}
}

#endif
