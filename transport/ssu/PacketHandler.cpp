#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport) :
			m_transport(transport) {}

		void PacketHandler::packetReceived(PacketPtr &p, PeerStatePtr &ps)
		{
			if(p->getData().size() < Packet::MIN_PACKET_LEN)
				return;

			if(ps) {
				// ...
			} else {
				handleNewPacket(p);
			}
		}

		void PacketHandler::handleNewPacket(PacketPtr &p)
		{
		}
	}
}
