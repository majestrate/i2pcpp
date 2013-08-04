#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport, SessionKey const &sk) :
			m_transport(transport),
			m_inboundKey(sk),
			m_imf(transport),
	 		m_log(boost::log::keywords::channel = "PH") {}

		void PacketHandler::packetReceived(PacketPtr p)
		{
			I2P_LOG_SCOPED_EP(m_log, p->getEndpoint());
			p->truncate();
			I2P_LOG(m_log,debug) << "packet truncated to " << p->getData().size() << " bytes"; 
			PeerStatePtr ps = m_transport.m_peers.getRemotePeer(p->getEndpoint());
			if(ps) {
				handlePacket(p, ps);
			} else {
				EstablishmentStatePtr es = m_transport.getEstablisher().getState(p->getEndpoint());
				if(es)
					handlePacket(p, es);
				else
					handlePacket(p);
			}
		}

		void PacketHandler::handlePacket(PacketPtr const &packet, PeerStatePtr const &state)
		{
			I2P_LOG_SCOPED_TAG(m_log, "HP_PS");

			if(!packet->verify(state->getCurrentMacKey())) {
				I2P_LOG(m_log, error) << "packet verification failed";
				return;
			}

			packet->decrypt(state->getCurrentSessionKey());
			ByteArray &data = packet->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			uint32_t timestamp = (*dataItr++);
			timestamp |= (*dataItr++) << 8;
			timestamp |= (*dataItr++) << 16;
			timestamp |= (*dataItr++) << 24;

			switch(ptype) {
				case Packet::DATA:
					I2P_LOG(m_log, debug) << "data packet received";
					m_imf.receiveData(state, dataItr, data.cend());
					break;

				case Packet::SESSION_DESTROY:
					I2P_LOG(m_log, debug) << "received session destroy";
					handleSessionDestroyed(state);
					break;
			}
		}

		void PacketHandler::handlePacket(PacketPtr const &packet, EstablishmentStatePtr const &state)
		{
			I2P_LOG_SCOPED_TAG(m_log, "HP_EM");

			if(!packet->verify(state->getMacKey())) {
				I2P_LOG(m_log, error) << "packet verification failed";
				return;
			}

			ByteArray &data = packet->getData();
			if(state->getDirection() == EstablishmentState::OUTBOUND)
				state->setIV(data.begin() + 16, data.begin() + 32);

			packet->decrypt(state->getSessionKey());
			data = packet->getData();

			auto begin = data.cbegin();
			unsigned char flag = *(begin++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			uint32_t timestamp = (*begin++);
			timestamp |= (*begin++) << 8;
			timestamp |= (*begin++) << 16;
			timestamp |= (*begin++) << 24;

			switch(ptype) {
				case Packet::SESSION_CREATED:
					handleSessionCreated(begin, data.cend(), state);
					break;

				case Packet::SESSION_CONFIRMED:
					handleSessionConfirmed(begin, data.cend(), state);
					break;

				case Packet::SESSION_DESTROY:
					I2P_LOG(m_log, debug) << "received session destroy";
					handleSessionDestroyed(state);
					break;
			}
		}

		void PacketHandler::handlePacket(PacketPtr &p)
		{
			I2P_LOG_SCOPED_TAG(m_log, "HP_P");

			Endpoint ep = p->getEndpoint();

			if(!p->verify(m_inboundKey)) {
				I2P_LOG(m_log, error) << "dropping new packet with invalid key";
				return;
			}

			p->decrypt(m_inboundKey);
			ByteArray &data = p->getData();

			auto dataItr = data.cbegin();
			auto end = data.cend();

			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			uint32_t timestamp = (*dataItr++);
			timestamp |= (*dataItr++) << 8;
			timestamp |= (*dataItr++) << 16;
			timestamp |= (*dataItr++) << 24;

			switch(ptype) {
				case Packet::SESSION_REQUEST:
					handleSessionRequest(dataItr, end, m_transport.getEstablisher().createState(ep));
					break;

				default:
					I2P_LOG(m_log, error) << "dropping new, out-of-state packet";
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

		void PacketHandler::handleSessionCreated(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state)
		{

			if(state->getState() != EstablishmentState::REQUEST_SENT) {
				I2P_LOG(m_log,warning) << "SCr invalid state: " << state->getState();
				return;
			}
			state->setTheirDH(begin, begin + 256), begin += 256;

			unsigned char ipSize = *(begin++);

			if(ipSize != 4 && ipSize != 16) {
				I2P_LOG(m_log,warning) << "invalid ip address size: " << ipSize;
				return;
			}

			ByteArray ip(begin, begin + ipSize);
			begin += ipSize;
			uint16_t port = (((*(begin++)) << 8) | (*(begin++)));

			state->setMyEndpoint(Endpoint(ip, port));

			uint32_t relayTag = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			state->setRelayTag(relayTag);

			uint32_t ts = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			state->setSignatureTimestamp(ts);

			state->setSignature(begin, begin + 48);

			state->setState(EstablishmentState::CREATED_RECEIVED);
			m_transport.getEstablisher().post(state);
		}

		void PacketHandler::handleSessionConfirmed(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state)
		{
			if(state->getState() != EstablishmentState::CREATED_SENT) {
				I2P_LOG(m_log,warning) << "SCf invalid state: " << state->getState();
				return;
			}

			unsigned char info = *(begin++);
			uint16_t size = (((*(begin++)) << 8) | (*(begin++)));
			(void)info; (void)size; // Stop compiler from complaining

			RouterIdentity ri(begin, end);
			state->setTheirIdentity(ri);

			uint32_t ts = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
			state->setSignatureTimestamp(ts);

			state->setSignature(end - 40, end);

			state->setState(EstablishmentState::CONFIRMED_RECEIVED);
			m_transport.getEstablisher().post(state);
		}

		void PacketHandler::handleSessionDestroyed(PeerStatePtr const &ps)
		{
			m_transport.m_peers.delRemotePeer(ps->getEndpoint());
			m_transport.post(boost::bind(boost::ref(m_transport.m_disconnectedSignal), ps->getIdentity().getHash()));
		}

		void PacketHandler::handleSessionDestroyed(EstablishmentStatePtr const &state)
		{
			state->setState(EstablishmentState::FAILURE);
			m_transport.getEstablisher().post(state);
		}
	}
}
