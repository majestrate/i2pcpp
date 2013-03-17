#ifndef SSUUDPTRANSPORT_H
#define SSUUDPTRANSPORT_H

#include <thread>

#include <boost/asio.hpp>

#include "../Transport.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport : public Transport {
			public:
				UDPTransport();
				~UDPTransport();

				void start(Endpoint const &ep);
				void connect(RouterHash const &rh, Endpoint const &ep);
				void send(RouterHash const &rh, ByteArray const &msg);
				void disconnect(RouterHash const &rh);
				void shutdown();

			private:
				boost::asio::io_service m_ios;
				boost::asio::ip::udp::socket m_socket;

				std::thread m_serviceThread;
		};
	}
}

#endif
