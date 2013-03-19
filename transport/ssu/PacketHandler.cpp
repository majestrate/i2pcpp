#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport) :
			m_transport(transport) {}

		void PacketHandler::packetReceived(PacketPtr &p, PeerStatePtr &ps)
		{
			m_transport.sendPacket(p);
		}
	}
}
