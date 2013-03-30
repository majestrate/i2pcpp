#ifndef SSUOUTBOUNDMESSAGEFRAGMENTS_H
#define SSUOUTBOUNDMESSAGEFRAGMENTS_H

#include "PeerState.h"
#include "OutboundMessageState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		typedef std::shared_ptr<boost::asio::deadline_timer> FragmentTimerPtr;

		class OutboundMessageFragments {
			public:
				OutboundMessageFragments(UDPTransport &transport);

				void sendData(PeerStatePtr const &ps, ByteArray const &data);

			private:
				void sendDataCallback(PeerStatePtr ps, uint32_t msgId);
				void fragmentTimerCallback(PeerStatePtr ps, uint32_t msgId, FragmentTimerPtr timer);

			private:
				UDPTransport &m_transport;
		};
	}
}

#endif
