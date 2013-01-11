#ifndef SSUACKNOWLEDGEMENTSCHEDULER_H
#define SSUACKNOWLEDGEMENTSCHEDULER_H

#include <boost/asio.hpp>

#include "InboundMessageState.h"
#include "OutboundMessageState.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;
		typedef std::shared_ptr<boost::asio::deadline_timer> AcknowledgementTimerPtr;

		class AcknowledgementScheduler : public boost::asio::io_service::service {
			public:
				AcknowledgementScheduler(UDPTransport &transport);

				void shutdown_service() {}
				
				AcknowledgementTimerPtr createInboundTimer(InboundMessageStatePtr ims);
				AcknowledgementTimerPtr createOutboundTimer(OutboundMessageStatePtr oms);
				void cancelTimer(AcknowledgementTimerPtr const &t);

			private:
				void flushAckCallback(const boost::system::error_code& e, AcknowledgementTimerPtr &timer);
				void inboundCallback(const boost::system::error_code& e, InboundMessageStatePtr ims);
				void outboundCallback(const boost::system::error_code& e, OutboundMessageStatePtr oms);

				UDPTransport& m_transport;
		};
	}
}

#endif
