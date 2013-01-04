#ifndef UDPSENDER_H
#define UDPSENDER_H

#include "../Thread.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class UDPSender : public Thread {
			public:
				UDPSender(UDPTransport &transport) : m_transport(transport) {}

			private:
				void loop();

				UDPTransport &m_transport;
		};
	}
}

#endif
