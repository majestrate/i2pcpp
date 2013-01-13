#include "PacketHandler.h"

#include <iostream>
#include <iomanip>

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		PacketHandler::PacketHandler(UDPTransport &transport) :
			m_transport(transport),
			m_inboundKey(transport.m_ctx.getMyRouterIdentity().getHash()),
			m_imf(transport) {}

		void PacketHandler::packetReceived(PacketPtr &p)
		{
			if(p->getData().size() < Packet::MIN_PACKET_LEN)
				return;

			PeerStatePtr ps = m_transport.m_peers.getRemotePeer(p->getEndpoint());

			if(ps)
				handlePacket(p, ps);
			else {
				EstablishmentStatePtr es = m_transport.m_establisher.getState(p->getEndpoint());
				if(es)
						handlePacket(p, es);
				else
					handleNewPacket(p);
			}
		}

		void PacketHandler::handlePacket(PacketPtr const &packet, PeerStatePtr const &state)
		{
			if(!packet->verify(state->getCurrentMacKey())) {
				std::cerr << "PacketHandler[PS]: packet verification failed from " << packet->getEndpoint().toString() << "\n";
				return;
			}

			packet->decrypt(state->getCurrentSessionKey());
			ByteArray &data = packet->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // Need to validate timestamp

			switch(ptype) {
				case Packet::DATA:
					std::cerr << "PacketHandler[PS]: SSU data packet received from " << state->getEndpoint().toString() << ":\n";
					m_imf.receiveData(state, dataItr);
					break;
			}
		}

		void PacketHandler::handlePacket(PacketPtr const &packet, EstablishmentStatePtr const &state)
		{
			if(!packet->verify(state->getMacKey())) {
				std::cerr << "PacketHandler[ES]: packet verification failed from " << packet->getEndpoint().toString() << "\n";
				return;
			}

			ByteArray &data = packet->getData();
			if(!state->isInbound())
				state->setIV(data.begin() + 16, data.begin() + 32);

			packet->decrypt(state->getSessionKey());
			data = packet->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // TODO validate timestamp

			switch(ptype) {
				case Packet::SESSION_CREATED:
					std::cerr << "PacketHandler[ES]: received session created from " << state->getTheirEndpoint().toString() << "\n";
					handleSessionCreated(dataItr, state);
					break;

				case Packet::SESSION_CONFIRMED:
					std::cerr << "PacketHandler[ES]: received session confirmed from " << state->getTheirEndpoint().toString() << "\n";
					handleSessionConfirmed(dataItr, data.cend(), state);
					break;
			}
		}

		void PacketHandler::handleNewPacket(PacketPtr const &packet)
		{
			Endpoint ep = packet->getEndpoint();

			if(!packet->verify(m_inboundKey)) {
				std::cerr << "PacketHandler[N]: packet verification failed from " << ep.toString() << "\n";
				return;
			}

			packet->decrypt(m_inboundKey);
			ByteArray &data = packet->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // TODO validate timestamp

			switch(ptype) {
				case Packet::SESSION_REQUEST:
					std::cerr << "PacketHandler[N]: received session request from " << ep.toString() << "\n";
					handleSessionRequest(dataItr, m_transport.m_establisher.establish(ep, m_inboundKey));
					break;
			}
		}

		void PacketHandler::handleSessionRequest(ByteArray::const_iterator &dataItr, EstablishmentStatePtr const &state)
		{
			state->setTheirDH(dataItr, dataItr + 256), dataItr += 256;

			unsigned char ipSize = *(dataItr++);

			if(ipSize != 4 && ipSize != 16)
				return;

			ByteArray ip(dataItr, dataItr + ipSize);
			dataItr += ipSize;
			short port = (((*(dataItr++)) << 8) | (*(dataItr++)));

			state->setMyEndpoint(Endpoint(ip, port));

			state->setRelayTag(0); // TODO Relay support

			state->requestReceived();
			m_transport.m_establisher.addWork(state);
		}

		void PacketHandler::handleSessionCreated(ByteArray::const_iterator &dataItr, EstablishmentStatePtr const &state)
		{
			if(state->getState() != EstablishmentState::REQUEST_SENT)
				return;

			state->setTheirDH(dataItr, dataItr + 256), dataItr += 256;

			unsigned char ipSize = *(dataItr++);

			if(ipSize != 4 && ipSize != 16)
				return;

			ByteArray ip(dataItr, dataItr + ipSize);
			dataItr += ipSize;
			uint16_t port = (((*(dataItr++)) << 8) | (*(dataItr++)));

			state->setMyEndpoint(Endpoint(ip, port));

			uint32_t relayTag = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
			state->setRelayTag(relayTag);

			uint32_t ts = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
			state->setSignatureTimestamp(ts);

			state->setSignature(dataItr, dataItr + 48);

			state->createdReceived();
			m_transport.m_establisher.addWork(state);
		}

		void PacketHandler::handleSessionConfirmed(ByteArray::const_iterator &dataItr, ByteArray::const_iterator end, EstablishmentStatePtr const &state)
		{
			if(state->getState() != EstablishmentState::CREATED_SENT)
				return;

			unsigned char info = *(dataItr++);
			uint16_t size = (((*(dataItr++)) << 8) | (*(dataItr++)));
			(void)info; (void)size; // Stop compiler from complaining

			RouterIdentity ri(dataItr);
			state->setTheirIdentity(ri);

			uint32_t ts = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
			state->setSignatureTimestamp(ts);

			state->setSignature(end - 40, end);

			state->confirmedReceived();
			m_transport.m_establisher.addWork(state);
		}
	}
}
