#include "PacketHandler.h"

#include <bitset>

#include "UDPTransport.h"
#include "InboundMessageFragments.h"
#include "MessageReceiver.h"

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		void PacketHandler::loop()
		{
			PacketQueue& pq = m_transport.getInboundQueue();
			EstablishmentManager& em = m_transport.getEstablisher();

			while(m_keepRunning) {
				pq.wait();
				PacketPtr p = pq.pop();

				if(!p)
					continue;

				if(p->getData().size() < Packet::MIN_PACKET_LEN)
					continue;

				PeerStatePtr ps = m_transport.getRemotePeer(p->getEndpoint());
				if(ps)
					handlePacket(p, ps);
				else {
					EstablishmentStatePtr es = em.getState(p->getEndpoint());
					if(es) {
						if(es->isInbound()) {
						} else
							handlePacketOutbound(p, es);
						em.addWork(es);
					} else
						std::cerr << "PacketHandler: no PeerState and no ES, dropping packet\n";
				}
			}
		}

		void PacketHandler::handlePacket(PacketPtr const &packet, PeerStatePtr const &state)
		{
			std::lock_guard<std::mutex> lock(state->getMutex());

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
					std::cerr << "PacketHandler[PS]: data received from " << state->getEndpoint().toString() << ":\n";
					m_imf.receiveData(dataItr, state);
					break;
			}

			/* Only temporary */
			/*PacketBuilder b;
			PacketPtr sdp = b.buildSessionDestroyed(state);
			sdp->encrypt(state->getCurrentSessionKey(), state->getCurrentMacKey());
			m_transport.send(sdp);*/
		}

		void PacketHandler::handlePacketOutbound(PacketPtr const &packet, EstablishmentStatePtr const &state)
		{
			std::lock_guard<std::mutex> lock(state->getMutex());

			if(!packet->verify(state->getMacKey())) {
				std::cerr << "PacketHandler[ES]: packet verification failed from " << packet->getEndpoint().toString() << "\n";
				return;
			}

			ByteArray &data = packet->getData();
			state->setIV(data.begin() + 16, data.begin() + 32);

			packet->decrypt(state->getSessionKey());
			data = packet->getData();

			auto dataItr = data.cbegin();
			unsigned char flag = *(dataItr++);
			Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

			dataItr += 4; // Need to validate timestamp

			switch(ptype) {
				case Packet::SESSION_CREATED:
					std::cerr << "PacketHandler[ES]: received session created from " << state->getTheirEndpoint().toString() << "\n";
					handleSessionCreated(dataItr, state);
					break;
			}
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
			short port = (((*(dataItr++)) << 8) | (*(dataItr++)));

			state->setMyEndpoint(Endpoint(ip, port));

			state->setRelayTag(dataItr, dataItr + 4), dataItr += 4;

			state->setSignatureTimestamp(dataItr, dataItr + 4), dataItr += 4;

			state->setSignature(dataItr, dataItr + 48);

			state->createdReceived();
		}
	}
}
