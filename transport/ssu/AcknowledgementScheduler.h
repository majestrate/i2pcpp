#ifndef SSUACKNOWLEDGEMENTSCHEDULER_H
#define SSUACKNOWLEDGEMENTSCHEDULER_H

#include <boost/asio.hpp>

#include "InboundMessageState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		typedef std::shared_ptr<boost::asio::deadline_timer> AcknowledgementTimerPtr;

		class AcknowledgementScheduler {
			public:
				AcknowledgementScheduler(UDPTransport &transport);

			private:
				void flushAckCallback(const boost::system::error_code& e, AcknowledgementTimerPtr &timer);

				UDPTransport& m_transport;
		};
	}
}

#endif
