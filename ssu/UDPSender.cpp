#include "UDPSender.h"

#include <boost/asio.hpp>

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		void UDPSender::loop()
		{
			try {
				PacketQueue& oq = m_transport.m_outboundQueue;
				boost::asio::ip::udp::socket& s = m_transport.m_socket;

				while(m_keepRunning) {
					PacketPtr p = oq.wait_and_pop();

					ByteArray pdata = p->getData();
					Endpoint ep = p->getEndpoint();

					if(s.is_open())
						s.send_to(boost::asio::buffer(pdata.data(), pdata.size()), ep.getUDPEndpoint());
				}
			} catch(LockingQueueFinished) {}
		}
	}
}
