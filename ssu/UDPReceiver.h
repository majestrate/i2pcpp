#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <array>

#include <boost/asio.hpp>

#include "../Thread.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class UDPReceiver : public Thread {
			public:
				UDPReceiver(UDPTransport &transport) : m_transport(transport)	{}

			private:
				void loop();

				UDPTransport &m_transport;
				boost::asio::ip::udp::endpoint m_endpoint;

				static const unsigned int BUFSIZE = 1024;
				std::array<unsigned char, BUFSIZE> m_receiveBuf;
		};
	}
}

#endif
