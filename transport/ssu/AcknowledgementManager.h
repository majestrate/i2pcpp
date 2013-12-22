#ifndef SSUACKNOWLEDGEMENTMANAGER_H
#define SSUACKNOWLEDGEMENTMANAGER_H

#include <boost/asio.hpp>

#include "../../Log.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class AcknowledgementManager {
			public:
				AcknowledgementManager(UDPTransport &transport);
				AcknowledgementManager(const AcknowledgementManager &) = delete;
				AcknowledgementManager& operator=(AcknowledgementManager &) = delete;

			private:
				void flushAckCallback(const boost::system::error_code& e);

				UDPTransport& m_transport;

				boost::asio::deadline_timer m_timer;

				i2p_logger_mt m_log;
		};
	}
}

#endif
