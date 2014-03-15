/**
 * @file PacketBuilder.cpp
 * @brief Implements PacketBuilder.h
 */
#include "PacketBuilder.h"

#include "Packet.h"
#include "EstablishmentState.h"

#include "../../Log.h"

#include <i2pcpp/datatypes/RouterIdentity.h>

#include <chrono>

namespace i2pcpp {
    namespace SSU {
        PacketPtr PacketBuilder::buildHeader(Endpoint const &ep, unsigned char flag)
        {
            auto s = std::make_shared<Packet>(ep);
            ByteArray& data = s->getData();

            data.insert(data.begin(), flag);

            uint32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            data.insert(data.end(), timestamp >> 24);
            data.insert(data.end(), timestamp >> 16);
            data.insert(data.end(), timestamp >> 8);
            data.insert(data.end(), timestamp);

            return s;
        }

        PacketPtr PacketBuilder::buildSessionRequest(EstablishmentStatePtr const &state)
        {
            PacketPtr s = buildHeader(state->getTheirEndpoint(), (unsigned char)Packet::PayloadType::SESSION_REQUEST << 4);

            ByteArray& sr = s->getData();

            const ByteArray&& myDH = state->getMyDH();
            sr.insert(sr.end(), myDH.begin(), myDH.end());

            ByteArray ip = state->getTheirEndpoint().getRawIP();
            sr.insert(sr.end(), (unsigned char)ip.size());
            sr.insert(sr.end(), ip.begin(), ip.end());
            uint16_t port = state->getTheirEndpoint().getPort();
            sr.insert(sr.end(), (port >> 8));
            sr.insert(sr.end(), port);

            return s;
        }

        PacketPtr PacketBuilder::buildSessionCreated(EstablishmentStatePtr const &state)
        {
            PacketPtr s = buildHeader(state->getTheirEndpoint(), (unsigned char)Packet::PayloadType::SESSION_CREATED << 4);

            ByteArray& sc = s->getData();

            const ByteArray&& myDH = state->getMyDH();
            sc.insert(sc.end(), myDH.begin(), myDH.end());

            ByteArray ip = state->getTheirEndpoint().getRawIP();
            sc.insert(sc.end(), (unsigned char)ip.size());
            sc.insert(sc.end(), ip.begin(), ip.end());
            uint16_t port = state->getTheirEndpoint().getPort();
            sc.insert(sc.end(), (port >> 8));
            sc.insert(sc.end(), port);

            uint32_t relayTag = state->getRelayTag();
            sc.insert(sc.end(), relayTag >> 24);
            sc.insert(sc.end(), relayTag >> 16);
            sc.insert(sc.end(), relayTag >> 8);
            sc.insert(sc.end(), relayTag);

            uint32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            sc.insert(sc.end(), timestamp >> 24);
            sc.insert(sc.end(), timestamp >> 16);
            sc.insert(sc.end(), timestamp >> 8);
            sc.insert(sc.end(), timestamp);

            const ByteArray&& signature = state->calculateCreationSignature(timestamp);
            sc.insert(sc.end(), signature.begin(), signature.end());

            return s;
        }

        PacketPtr PacketBuilder::buildSessionConfirmed(EstablishmentStatePtr const &state)
        {
            PacketPtr s = buildHeader(state->getTheirEndpoint(), (unsigned char)Packet::PayloadType::SESSION_CONFIRMED << 4);

            ByteArray& sc = s->getData();

            sc.insert(sc.end(), 0x01);

            ByteArray idBytes = state->getMyIdentity().serialize();
            uint16_t size = idBytes.size();
            sc.insert(sc.end(), size >> 8);
            sc.insert(sc.end(), size);

            sc.insert(sc.end(), idBytes.begin(), idBytes.end());

            uint32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            sc.insert(sc.end(), timestamp >> 24);
            sc.insert(sc.end(), timestamp >> 16);
            sc.insert(sc.end(), timestamp >> 8);
            sc.insert(sc.end(), timestamp);

            sc.insert(sc.end(), 9, 0x00); // TODO Real padding?

            const ByteArray&& signature = state->calculateConfirmationSignature(timestamp);
            sc.insert(sc.end(), signature.begin(), signature.end());

            return s;
        }

        PacketPtr PacketBuilder::buildData(Endpoint const &ep, bool wantReply, CompleteAckList const &completeAcks, PartialAckList const &incompleteAcks, std::vector<PacketBuilder::FragmentPtr> const &fragments)
        {
            PacketPtr s = buildHeader(ep, (unsigned char)Packet::PayloadType::DATA << 4);

            ByteArray& d = s->getData();

            unsigned char dataFlag = 0;

            if(wantReply)
                dataFlag |= (1 << 2);

            ByteArray ea(1), ba(1);
            ea[0] = 0; ba[0] = 0;

            for(auto m: completeAcks) {
                ea.insert(ea.end(), m >> 24);
                ea.insert(ea.end(), m >> 16);
                ea.insert(ea.end(), m >> 8);
                ea.insert(ea.end(), m);
                ea[0]++;
            }

            for(auto m: incompleteAcks) {
                ba.insert(ba.end(), m.first >> 24);
                ba.insert(ba.end(), m.first >> 16);
                ba.insert(ba.end(), m.first >> 8);
                ba.insert(ba.end(), m.first);

                size_t numBits = m.second.size();
                size_t steps = std::ceil(numBits / 7.0);

                for(size_t i = 0; i < steps; i++) {
                    uint8_t byte = 0;

                    if((i + 1) < steps)
                        byte |= (1 << 7);

                    for(int j = 6, k = (i * 7); j >= 0 && k < numBits; j--, k++) {
                        if(m.second[k])
                            byte |= (1 << j);
                    }

                    ba.insert(ba.end(), byte);
                }

                ba[0]++;
            }

            if(ea[0])
                dataFlag |= (1 << 7);

            if(ba[0])
                dataFlag |= (1 << 6);

            d.insert(d.end(), dataFlag);

            if(ea[0])
                d.insert(d.end(), ea.cbegin(), ea.cend());

            if(ba[0])
                d.insert(d.end(), ba.cbegin(), ba.cend());

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

                if(f->data.size() > 16383)
                    throw std::logic_error("fragment size too big");

                fragInfo |= (f->data.size());

                d.insert(d.end(), fragInfo >> 16);
                d.insert(d.end(), fragInfo >> 8);
                d.insert(d.end(), fragInfo);

                d.insert(d.end(), f->data.cbegin(), f->data.cend());
            }

            return s;
        }

        PacketPtr PacketBuilder::buildSessionDestroyed(Endpoint const &ep)
        {
            PacketPtr s = buildHeader(ep, (unsigned char)Packet::PayloadType::SESSION_DESTROY << 4);

            return s;
        }
    }
}
