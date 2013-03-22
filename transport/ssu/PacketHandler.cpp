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
			auto end = data.cend();

			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // TODO validate timestamp

			switch(ptype) {
				case Packet::SESSION_REQUEST:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "received session request";
					handleSessionRequest(dataItr, end, m_transport.getEstablisher().createState(ep));
					break;
				default:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "dropping new packet";
			}
		}

		void PacketHandler::handleSessionRequest(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state)
		{
			state->setTheirDH(begin, begin + 256), begin += 256;

			unsigned char ipSize = *(begin++);

			if(ipSize != 4 && ipSize != 16)
				return;

			ByteArray ip(begin, begin + ipSize);
			begin += ipSize;
			short port = (((*(begin++)) << 8) | (*(begin++)));

			state->setMyEndpoint(Endpoint(ip, port));

			state->setRelayTag(0); // TODO Relay support

			state->setState(EstablishmentState::REQUEST_RECEIVED);
			m_transport.getEstablisher().post(state);
		}
	}
}
