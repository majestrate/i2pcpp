#ifndef UDPSENDER_H
#define UDPSENDER_H

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class UDPSender {
			public:
				UDPSender(UDPTransport &transport) : m_transport(transport) {}

				void run();

			private:
				UDPTransport &m_transport;
		};
	}
}

#endif
