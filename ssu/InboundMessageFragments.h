#ifndef SSUINBOUNDMESSAGEFRAGMENTS_H
#define SSUINBOUNDMESSAGEFRAGMENTS_H

#include <thread>

#include "PeerState.h"
#include "MessageReceiver.h"

#include "../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class InboundMessageFragments {
			public:
				InboundMessageFragments(UDPTransport &transport);

				void begin() { m_messageReceiver.start(); }
				void shutdown() { m_messageReceiver.stop(); }
				void receiveData(PeerStatePtr const &ps, ByteArray::const_iterator &dataItr);

			private:
				UDPTransport &m_transport;
				// TODO Decaying bloom filter
				MessageReceiver m_messageReceiver;
		};
	}
}

#endif
