#include "UDPTransport.h"

#include <boost/exception/all.hpp>

namespace i2pcpp {
	namespace SSU {
		UDPTransport::UDPTransport() :
			m_socket(m_ios)
		{
		}

		UDPTransport::~UDPTransport()
		{
			shutdown();
		}

		void UDPTransport::start(Endpoint const &ep)
		{
			try {
				if(ep.getUDPEndpoint().address().is_v4())
					m_socket.open(boost::asio::ip::udp::v4());
				else if(ep.getUDPEndpoint().address().is_v6())
					m_socket.open(boost::asio::ip::udp::v6());

				m_socket.bind(ep.getUDPEndpoint());

				m_serviceThread = std::thread([&](){
					try {
						while(1) { m_ios.run(); break; }
					} catch(std::exception &e) {
						// TODO Handle exception
					}
				});
			} catch(boost::system::system_error &e) {
				shutdown();
				throw;
			}
		}

		void UDPTransport::connect(RouterHash const &rh, Endpoint const &ep)
		{
		}

		void UDPTransport::send(RouterHash const &rh, ByteArray const &msg)
		{
		}

		void UDPTransport::disconnect(RouterHash const &rh)
		{
		}

		void UDPTransport::shutdown()
		{
			m_ios.stop();
			if(m_serviceThread.joinable()) m_serviceThread.join();
		}
	}
}
