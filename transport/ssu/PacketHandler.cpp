#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport, SessionKey const &sk) :
			m_transport(transport),
			m_inboundKey(sk) {}

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

			if(!p->verify(m_inboundKey)) {
				BOOST_LOG_SEV(m_transport.getLogger(), debug) << "dropping new packet with invalid key";
				return;
			}

			p->decrypt(m_inboundKey);
			ByteArray &data = p->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // TODO validate timestamp

			switch(ptype) {
				case Packet::SESSION_REQUEST:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "received session request";
					// handleSessionRequest
					break;
				default:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "dropping new packet";
			}
		}
	}
}
