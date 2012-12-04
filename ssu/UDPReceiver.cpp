#include "UDPReceiver.h"

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		void UDPReceiver::run()
		{
			ip::udp::socket& s =  m_transport.getSocket();
			PacketQueue& iq = m_transport.getInboundQueue();

			while(m_transport.keepRunning()) {
				size_t len = s.receive_from(buffer(m_receiveBuf.data(), BUFSIZE), m_endpoint);

				if(len > 0) {
					PacketPtr p(new Packet(Endpoint(m_endpoint), m_receiveBuf.data(), len));
					iq.enqueue(p);
				}
			}
		}
	}
}
