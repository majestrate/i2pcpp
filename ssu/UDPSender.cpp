#include "UDPSender.h"

#include <boost/asio.hpp>

#include "UDPTransport.h"

using namespace boost::asio;

namespace i2pcpp {
	namespace SSU {
		void UDPSender::run()
		{
			PacketQueue& oq = m_transport.getOutboundQueue();

			while(m_transport.keepRunning()) {
				oq.wait();

				PacketPtr p = oq.pop();
				if(!p)
					return;

				ByteArray pdata = p->getData();
				Endpoint ep = p->getEndpoint();

				ip::udp::socket& s = m_transport.getSocket();
				if(s.is_open())
					s.send_to(buffer(pdata.data(), pdata.size()), ep.getUDPEndpoint());
			}
		}
	}
}
