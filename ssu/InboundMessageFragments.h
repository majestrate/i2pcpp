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
				InboundMessageFragments(UDPTransport &transport) : m_messageReceiver(transport) {}

				void begin();
				void join();
				void receiveData(ByteArray::const_iterator &dataItr, PeerStatePtr const &ps);

			private:
				void startMessageReceiver();

				// TODO: Decaying bloom filter
				MessageReceiver m_messageReceiver;
				thread m_messageReceiverThread;
		};
	}
}

#endif
