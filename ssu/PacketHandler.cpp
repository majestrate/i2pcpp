#include "PacketHandler.h"

#include "UDPTransport.h"

#include <iostream>
#include "../util/Base64.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		void PacketHandler::run()
		{
			PacketQueue &pq = m_transport.getInboundQueue();
			EstablishmentManager &em = m_transport.getEstablisher();

			while(m_transport.keepRunning()) {
				pq.wait();
				PacketPtr p = pq.pop();

				if(!p)
					continue;

				ByteArray &data = p->getData();

				if(data.size() < Packet::MIN_PACKET_LEN)
					continue;

				OutboundEstablishmentStatePtr state = em.getOutboundState(p->getEndpoint());
				state->lock();

				if(!p->verify(state->getMacKey())) {
					cerr << "PacketHandler: packet verification failed from " << p->getEndpoint().toString() << "\n";
					state->unlock();
					continue;
				}

				state->setIV(data.begin() + 16, data.begin() + 32);

				p->decrypt(state->getSessionKey());
				data = p->getData();

				auto dataItr = data.begin();
				unsigned char flag = *(dataItr++);
				Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

				dataItr += 4; // Need to validate timestamp

				switch(ptype) {
					case Packet::SESSION_CREATED:
						cerr << "PacketHandler: received session created from " << state->getEndpoint().toString() << "\n";
						handleSessionCreated(dataItr, state);
						break;
				}

				state->unlock();
			}
		}

		void PacketHandler::handleSessionCreated(ByteArray::iterator dataItr, OutboundEstablishmentStatePtr const &state)
		{
			if(state->getState() != OutboundEstablishmentState::REQUEST_SENT)
				return;

			state->setDHY(dataItr, dataItr + 256), dataItr += 256;

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
