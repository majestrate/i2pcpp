#ifndef SSUPACKETBUILDER_H
#define SSUPACKETBUILDER_H

#include "Packet.h"
#include "EstablishmentState.h"

#include "../../datatypes/Endpoint.h"

namespace i2pcpp {
    namespace SSU {
        typedef std::vector<uint32_t> CompleteAckList;
        typedef std::map<uint32_t, std::vector<bool>> PartialAckList;

        class PacketBuilder {
            public:
                struct Fragment {
                    uint32_t msgId;
                    bool isLast;
                    uint8_t fragNum;
                    ByteArray data;
                };
                typedef std::shared_ptr<Fragment> FragmentPtr;

                static PacketPtr buildSessionRequest(EstablishmentStatePtr const &state);
                static PacketPtr buildSessionCreated(EstablishmentStatePtr const &state);
                static PacketPtr buildSessionConfirmed(EstablishmentStatePtr const &state);
                static PacketPtr buildData(Endpoint const &ep, bool wantReply, CompleteAckList const &completeAcks, PartialAckList const &incompleteAcks, std::vector<FragmentPtr> const &fragments);
                static PacketPtr buildSessionDestroyed(Endpoint const &ep);

            private:
                static PacketPtr buildHeader(Endpoint const &ep, unsigned char flag);
        };
    }
}

#endif
