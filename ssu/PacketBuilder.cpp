#include "PacketBuilder.h"

#include <chrono>

#include "../RouterContext.h"
#include "../datatypes/RouterIdentity.h"

#include <iostream>

namespace i2pcpp {
	namespace SSU {
		PacketPtr PacketBuilder::buildHeader(Endpoint const &ep, unsigned char flag) const
		{
			PacketPtr s(new Packet(ep));
			ByteArray& data = s->getData();

			data.insert(data.begin(), flag);

			uint32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			data.insert(data.end(), timestamp >> 24);
			data.insert(data.end(), timestamp >> 16);
			data.insert(data.end(), timestamp >> 8);
			data.insert(data.end(), timestamp);

			return s;
		}

		PacketPtr PacketBuilder::buildSessionRequest(EstablishmentStatePtr const &state) const
		{
			PacketPtr s = buildHeader(state->getTheirEndpoint(), Packet::PayloadType::SESSION_REQUEST << 4);

			ByteArray& sr = s->getData();

			ByteArray myDH = state->getMyDH();
			sr.insert(sr.end(), myDH.begin(), myDH.end());

			ByteArray ip = state->getTheirEndpoint().getRawIP();
			sr.insert(sr.end(), (unsigned char)ip.size());
			sr.insert(sr.end(), ip.begin(), ip.end());
			uint16_t port = state->getTheirEndpoint().getPort();
			sr.insert(sr.end(), (port >> 8));
			sr.insert(sr.end(), port);

			return s;
		}

		PacketPtr PacketBuilder::buildSessionConfirmed(EstablishmentStatePtr const &state) const
		{
			PacketPtr s = buildHeader(state->getTheirEndpoint(), Packet::PayloadType::SESSION_CONFIRMED << 4);

			ByteArray& sc = s->getData();

			sc.insert(sc.end(), 0x01);

			ByteArray idBytes = state->getContext().getMyRouterIdentity().getBytes();
			uint16_t size = idBytes.size();
			sc.insert(sc.end(), size >> 8);
			sc.insert(sc.end(), size);

			sc.insert(sc.end(), idBytes.begin(), idBytes.end());

			uint32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			sc.insert(sc.end(), timestamp >> 24);
			sc.insert(sc.end(), timestamp >> 16);
			sc.insert(sc.end(), timestamp >> 8);
			sc.insert(sc.end(), timestamp);

			sc.insert(sc.end(), 9, 0x00);

			ByteArray signature = state->calculateConfirmationSignature(timestamp);
			sc.insert(sc.end(), signature.begin(), signature.end());

			return s;
		}

		PacketPtr PacketBuilder::buildSessionDestroyed(PeerStatePtr const &state) const
		{
			PacketPtr s = buildHeader(state->getEndpoint(), Packet::PayloadType::SESSION_DESTROY << 4);

			return s;
		}

		PacketPtr PacketBuilder::buildData(PeerStatePtr const &ps, bool wantReply, std::forward_list<OutboundMessageState::FragmentPtr> const &fragments) const
		{
			PacketPtr s = buildHeader(ps->getEndpoint(), Packet::PayloadType::DATA << 4);

			ByteArray& d = s->getData();

			unsigned char dataFlag = 0;

			if(wantReply)
				dataFlag |= (1 << 2);

			std::vector<uint32_t> toAck;
			for(int i = 0; i < 8; i++) {
				uint32_t ack = ps->popAck();
				if(ack) toAck.push_back(ack);
			}

/*			if(toAck.size()) {
				dataFlag |= (1 << 7);

				d.insert(d.end(), toAck.size());
				for(auto mid: toAck) {
					d.insert(d.end(), mid >> 24);
					d.insert(d.end(), mid >> 16);
					d.insert(d.end(), mid >> 8);
					d.insert(d.end(), mid);
					std::cerr << "PacketBuilder: appended ack: " << mid << "\n";
				}
			}*/

			d.insert(d.end(), dataFlag);

			d.insert(d.end(), distance(fragments.cbegin(), fragments.cend()));

			for(auto f: fragments) {
				d.insert(d.end(), f->msgId >> 24);
				d.insert(d.end(), f->msgId >> 16);
				d.insert(d.end(), f->msgId >> 8);
				d.insert(d.end(), f->msgId);

				uint32_t fragInfo = 0;

				fragInfo |= f->fragNum << 17;

				if(f->isLast)
					fragInfo |= (1 << 16);

				// TODO Exception if fragment size > 16383 (maybe)
				fragInfo |= (f->data.size());

				d.insert(d.end(), fragInfo >> 16);
				d.insert(d.end(), fragInfo >> 8);
				d.insert(d.end(), fragInfo);

				d.insert(d.end(), f->data.cbegin(), f->data.cend());
			}

			return s;
		}
	}
}
