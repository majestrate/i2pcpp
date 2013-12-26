#include "PacketHandler.h"

#include "../UDPTransport.h"

namespace i2pcpp {
    namespace SSU {
        PacketHandler::PacketHandler(UDPTransport &transport, SessionKey const &sk) :
            m_transport(transport),
            m_inboundKey(sk),
            m_imf(transport),
             m_log(I2P_LOG_CHANNEL("PH")) {}

        void PacketHandler::packetReceived(PacketPtr p)
        {
            I2P_LOG_SCOPED_TAG(m_log, "Endpoint", p->getEndpoint());

            auto ep = p->getEndpoint();

            std::lock_guard<std::mutex> lock(m_transport.m_peers.getMutex());

            if(m_transport.m_peers.peerExists(ep)) {
                handlePacket(p, m_transport.m_peers.getPeer(ep));
            } else {
                EstablishmentStatePtr es = m_transport.getEstablisher().getState(ep);
                if(es)
                    handlePacket(p, es);
                else
                    handlePacket(p);
            }
        }

        void PacketHandler::handlePacket(PacketPtr const &packet, PeerState const &state)
        {
            if(!packet->verify(state.getCurrentMacKey())) {
                I2P_LOG(m_log, error) << "packet verification failed";
                return;
            }

            packet->decrypt(state.getCurrentSessionKey());
            ByteArray &data = packet->getData();

            auto dataItr = data.cbegin();
            unsigned char flag = *(dataItr++);
            Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

            dataItr += 4; // TODO validate timestamp

            switch(ptype) {
                case Packet::PayloadType::DATA:
                    I2P_LOG(m_log, debug) << "data packet received";
                    m_imf.receiveData(state.getHash(), dataItr, data.cend());
                    break;

                case Packet::PayloadType::SESSION_DESTROY:
                    I2P_LOG(m_log, debug) << "received session destroy";
                    handleSessionDestroyed(state);
                    break;

                default:
                    break;
            }
        }

        void PacketHandler::handlePacket(PacketPtr const &packet, EstablishmentStatePtr const &state)
        {
            if(!packet->verify(state->getMacKey())) {
                I2P_LOG(m_log, error) << "packet verification failed";
                return;
            }

            ByteArray &data = packet->getData();
            if(state->getDirection() == EstablishmentState::Direction::OUTBOUND)
                state->setIV(data.begin() + 16, data.begin() + 32);

            packet->decrypt(state->getSessionKey());
            data = packet->getData();

            auto begin = data.cbegin();
            unsigned char flag = *(begin++);
            Packet::PayloadType ptype = (Packet::PayloadType)(flag >> 4);

            begin += 4; // TODO validate timestamp

            switch(ptype) {
                case Packet::PayloadType::SESSION_CREATED:
                    handleSessionCreated(begin, data.cend(), state);
                    break;

                case Packet::PayloadType::SESSION_CONFIRMED:
                    handleSessionConfirmed(begin, data.cend(), state);
                    break;

                case Packet::PayloadType::SESSION_DESTROY:
                    I2P_LOG(m_log, debug) << "received session destroy";
                    handleSessionDestroyed(state);
                    break;

                default:
                    break;
            }
        }

        void PacketHandler::handlePacket(PacketPtr &p)
        {
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

            dataItr += 4; // TODO validate timestamp

            switch(ptype) {
                case Packet::PayloadType::SESSION_REQUEST:
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

            state->setState(EstablishmentState::State::REQUEST_RECEIVED);
            m_transport.getEstablisher().post(state);
        }

        void PacketHandler::handleSessionCreated(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state)
        {
            if(state->getState() != EstablishmentState::State::REQUEST_SENT)
                return;

            state->setTheirDH(begin, begin + 256), begin += 256;

            unsigned char ipSize = *(begin++);

            if(ipSize != 4 && ipSize != 16)
                return;

            ByteArray ip(begin, begin + ipSize);
            begin += ipSize;
            uint16_t port = (((*(begin++)) << 8) | (*(begin++)));

            state->setMyEndpoint(Endpoint(ip, port));

            uint32_t relayTag = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
            state->setRelayTag(relayTag);

            uint32_t ts = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
            state->setSignatureTimestamp(ts);

            state->setSignature(begin, begin + 48);

            state->setState(EstablishmentState::State::CREATED_RECEIVED);
            m_transport.getEstablisher().post(state);
        }

        void PacketHandler::handleSessionConfirmed(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state)
        {
            if(state->getState() != EstablishmentState::State::CREATED_SENT)
                return;

            unsigned char info = *(begin++);
            uint16_t size = (((*(begin++)) << 8) | (*(begin++)));
            (void)info; (void)size; // Stop compiler from complaining

            RouterIdentity ri(begin, end);
            state->setTheirIdentity(ri);

            uint32_t ts = (*(begin++) << 24) | (*(begin++) << 16) | (*(begin++) << 8) | *(begin++);
            state->setSignatureTimestamp(ts);

            state->setSignature(end - 40, end);

            state->setState(EstablishmentState::State::CONFIRMED_RECEIVED);
            m_transport.getEstablisher().post(state);
        }

        void PacketHandler::handleSessionDestroyed(PeerState const &ps)
        {
            m_transport.m_peers.delPeer(ps.getEndpoint());
            m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_disconnectedSignal), ps.getHash()));
        }

        void PacketHandler::handleSessionDestroyed(EstablishmentStatePtr const &state)
        {
            state->setState(EstablishmentState::State::FAILURE);
            m_transport.getEstablisher().post(state);
        }
    }
}
