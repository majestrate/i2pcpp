#ifndef SSUACKNOWLEDGEMENTSCHEDULER_H
#define SSUACKNOWLEDGEMENTSCHEDULER_H

#include <boost/asio.hpp>
#include "../../Log.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class AcknowledgementScheduler {
			public:
				AcknowledgementScheduler(UDPTransport &transport);
				AcknowledgementScheduler(const AcknowledgementScheduler &) = delete;
				AcknowledgementScheduler& operator=(AcknowledgementScheduler &) = delete;

			private:
				void flushAckCallback(const boost::system::error_code& e);
				
				UDPTransport& m_transport;

				boost::asio::deadline_timer m_timer;
			
				i2p_logger_mt m_log;
		};
	}
}

#endif
