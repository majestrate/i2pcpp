#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport) :
			m_transport(transport) {}

		void PacketHandler::packetReceived(PacketPtr &p, PeerStatePtr &ps)
		{
			I2P_LOG_EP(m_transport.getLogger(), p->getEndpoint());

			if(p->getData().size() < Packet::MIN_PACKET_LEN) {
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "dropping short packet";
				return;
			}

			if(ps) {
				// ...
			} else {
				handleNewPacket(p);
			}
		}

		void PacketHandler::handleNewPacket(PacketPtr &p)
		{
			Endpoint ep = p->getEndpoint();

			if(!p->verify(m_transport.getInboundKey())) {
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "dropping new packet with invalid key";
				return;
			}
		}
	}
}
