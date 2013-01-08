#ifndef SSUACKNOWLEDGEMENTSCHEDULER_H
#define SSUACKNOWLEDGEMENTSCHEDULER_H

#include <boost/asio.hpp>

#include "../Thread.h"
#include "../util/LockingQueue.h"

#include "InboundMessageState.h"
#include "OutboundMessageState.h"
#include "PeerState.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;
		typedef std::shared_ptr<boost::asio::deadline_timer> AcknowledgementTimerPtr;

		class AcknowledgementScheduler : public Thread {
			public:
				AcknowledgementScheduler(UDPTransport &transport) : m_transport(transport) {}
				
				AcknowledgementTimerPtr createInboundTimer(InboundMessageStatePtr ims);
				AcknowledgementTimerPtr createOutboundTimer(OutboundMessageStatePtr oms);
				void cancelTimer(AcknowledgementTimerPtr const &t);

			private:
				void loop();
				void stopHook() { m_ios.stop(); }

				void flushAckCallback(const boost::system::error_code& e, AcknowledgementTimerPtr &timer);
				void inboundCallback(const boost::system::error_code& e, InboundMessageStatePtr ims);
				void outboundCallback(const boost::system::error_code& e, OutboundMessageStatePtr oms);

				boost::asio::io_service m_ios;
				UDPTransport& m_transport;
		};
	}
}

#endif
