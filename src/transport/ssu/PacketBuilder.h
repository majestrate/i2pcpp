/**
 * @file PacketBuilder.h
 * @brief Defines the i2pcpp::SSU::PacketBuilder class.
 */
#ifndef SSUPACKETBUILDER_H
#define SSUPACKETBUILDER_H

#include "Packet.h"
#include "EstablishmentState.h"

#include "../../datatypes/Endpoint.h"

namespace i2pcpp {
    namespace SSU {
        typedef std::vector<uint32_t> CompleteAckList;
        typedef std::map<uint32_t, std::vector<bool>> PartialAckList;

        /**
         * Class with static methods to build i2pcpp::SSU::Packet objects.
         */
        class PacketBuilder {
            public:
                /**
                 * Stores data about a fragment of an i2pcpp::SSU::Packet object.
                 */
                struct Fragment {
                    uint32_t msgId;
                    bool isLast;
                    uint8_t fragNum;
                    ByteArray data;
                };
                typedef std::shared_ptr<Fragment> FragmentPtr;

                /**
                 * Builds a session request packet.
                 * @param state a pointer to the associated i2pcpp::EstablishmentState
                 * @return a pointer to the newly created packet
                 */
                static PacketPtr buildSessionRequest(EstablishmentStatePtr const &state);

                /**
                 * Builds a session created packet.
                 * @param state a pointer to the associated i2pcpp::EstablishmentState
                 * @return a pointer to the newly created packet
                 */
                static PacketPtr buildSessionCreated(EstablishmentStatePtr const &state);

                /**
                 * Builds a session confirmed packet.
                 * @param state a pointer to the associated i2pcpp::EstablishmentState
                 * @return a pointer to the newly created packet
                 */
                static PacketPtr buildSessionConfirmed(EstablishmentStatePtr const &state);

                /**
                 * Builds a data packet.
                 * @param ep the remote i2pcpp::Endpoint
                 * @param wantReply is a reply wanted, currently should always be set
                 * @param completeAcks list of fully ACKed packages to be send
                 * @param partialAckList list of partially ACKed packages to be send
                 * @param fragments fragments of the data to be send
                 * @return a pointer to the newly created packet
                 */
                static PacketPtr buildData(Endpoint const &ep, bool wantReply, CompleteAckList const &completeAcks, PartialAckList const &incompleteAcks, std::vector<FragmentPtr> const &fragments);

                /**
                 * Builds a session destroyed packet.
                 * @param state a pointer to the associated i2pcpp::EstablishmentState
                 * @return a pointer to the newly created packet
                 */
                static PacketPtr buildSessionDestroyed(Endpoint const &ep);

            private:
                /**
                 * Builds the (encrypted payload) header of an unknown SSU packet.
                 * This consists of a 1 byte flag and a timestamp.
                 * @param flag should contain the following bitfields:
                 *  0-1: reserved;
                 *  2: indicates whether extended options are included;
                 *  3: indicates whether rekey data is included
                 *  4-7: the payload type (0 - 8)
                 * @note rekeying is not yet implemented
                 */
                static PacketPtr buildHeader(Endpoint const &ep, unsigned char flag);
        };
    }
}

#endif
