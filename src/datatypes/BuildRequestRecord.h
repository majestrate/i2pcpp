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

#include "../tunnel/TunnelHop.h"

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
             * @return an i2pcpp::TunnelHop object constructed from this build
             *  request record
             */
            TunnelHop parse();

            /**
             * Constructs an i2pcpp::BuildRequestRecord from an i2pcpp::TunnelHop
             *  object.
             * @todo implement random padding
             */
            void compile(TunnelHop const &hop);

            /**
             * Returns a reference to the associated i2pcpp::TunnelHop.
             */
            TunnelHop& getHop();

        private:
            /**
             * The flags to be used. Bit 7 indicates that the receiver router
             *  should act as an IBGW. Bit 6 indicates that it should act as
             *  and OBEP. If neither is set, this indicates that it should act
             *  as a participant.
             */
            std::bitset<8> m_flags;
    };

    typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
