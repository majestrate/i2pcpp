/**
 * @file BuildRequestRecord.h
 * @brief Defines the i2pcpp::BuildRequestRecord type.
 */
#ifndef BUILDREQUESTRECORD_H
#define BUILDREQUESTRECORD_H

#include <memory>

#include <botan/elgamal.h>

#include "Datatype.h"
#include "ByteArray.h"
#include "BuildRecord.h"
#include "RouterHash.h"

namespace i2pcpp {
    /**
     * Holds the tunnel build request record.
     * @see i2pcpp::I2NP::VariableTunnelBuild
     */
    class BuildRequestRecord : public BuildRecord {
        public:
            BuildRequestRecord() = default;

            /**
             * Constructs from an i2pcpp::BuilRecord.
             */
            BuildRequestRecord(BuildRecord const &r);

            /**
             * Parses the BuildRecord data. This must be called prior to using
             * the accessors for this class.
             */
            void parse();

            /**
             * Compiles the data assigned by the modifiers. This must be called
             * prior to encryption/decryption operations.
             * @todo implement random padding
             */
            void compile();

        private:
            uint32_t m_tunnelId;
            uint32_t m_nextTunnelId;
            RouterHash m_localHash;
            RouterHash m_nextHash;
            SessionKey m_tunnelLayerKey;
            SessionKey m_tunnelIVKey;
            SessionKey m_replyKey;
            StaticByteArray<16> m_replyIV;

            /**
             * The flags to be used. Bit 7 indicates that the receiver router
             *  should act as an IBGW. Bit 6 indicates that it should act as
             *  and OBEP. If neither is set, this indicates that it should act
             *  as a participant.
             */
            std::bitset<8> m_flags;

            uint32_t m_requestTime;
            uint32_t m_nextMsgId;

    };

    typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
