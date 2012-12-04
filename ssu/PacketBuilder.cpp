#include "PacketBuilder.h"

#include "../datatypes/RouterIdentity.h"

#include <chrono>

using namespace chrono;

namespace i2pcpp {
	namespace SSU {
		PacketPtr PacketBuilder::buildHeader(Endpoint const &ep, unsigned char flag) const
		{
			PacketPtr s(new Packet(ep));
			ByteArray& data = s->getData();

			data.insert(data.begin(), flag);

			unsigned int timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
			data.insert(data.end(), timestamp >> 24);
			data.insert(data.end(), timestamp >> 16);
			data.insert(data.end(), timestamp >> 8);
			data.insert(data.end(), timestamp);

			return s;
		}

		PacketPtr PacketBuilder::buildSessionRequest(OutboundEstablishmentStatePtr const &state) const
		{
			PacketPtr s = buildHeader(state->getEndpoint(), Packet::PayloadType::SESSION_REQUEST << 4);

			ByteArray& sr = s->getData();

			ByteArray dhx = state->getDHX();
			sr.insert(sr.end(), dhx.begin(), dhx.end());

			ByteArray ip = state->getEndpoint().getRawIP();
			sr.insert(sr.end(), (unsigned char)ip.size());
			sr.insert(sr.end(), ip.begin(), ip.end());
			unsigned short port = state->getEndpoint().getPort();
			sr.insert(sr.end(), (port >> 8));
			sr.insert(sr.end(), port);

			return s;
		}

		PacketPtr PacketBuilder::buildSessionConfirmed(OutboundEstablishmentStatePtr const &state) const
		{
			PacketPtr s = buildHeader(state->getEndpoint(), Packet::PayloadType::SESSION_CONFIRMED << 4);

			ByteArray& sc = s->getData();

			sc.insert(sc.end(), 0x01);

			ByteArray idBytes = state->getIdentity().getBytes();
			unsigned short size = idBytes.size();
			sc.insert(sc.end(), size >> 8);
			sc.insert(sc.end(), size);

			sc.insert(sc.end(), idBytes.begin(), idBytes.end());

			unsigned int timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
			sc.insert(sc.end(), timestamp >> 24);
			sc.insert(sc.end(), timestamp >> 16);
			sc.insert(sc.end(), timestamp >> 8);
			sc.insert(sc.end(), timestamp);

			sc.insert(sc.end(), 0x00, 8);

			ByteArray signature = state->calculateConfirmationSignature(timestamp);
			sc.insert(sc.end(), signature.begin(), signature.end());

			return s;
		}
	}
}
