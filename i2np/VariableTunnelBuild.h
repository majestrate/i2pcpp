/**
 * @file VariableTunnelBuild.h
 * @brief Defines the i2pcpp::I2NP::VariableTunnelBuild message type.
 */
#ifndef I2NPVARIABLETUNNELBUILD_H
#define I2NPVARIABLETUNNELBUILD_H

#include <list>

#include "../datatypes/BuildRecord.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a variable tunnel build message. These are message used
         *  tunnel creation. They are similar to the old TunnelBuild messages,
         *  but they contain a variable amount (1 - 8) of request records. 
         * The messages are send to all peers in the tunnel, and are rewritten
         *  in-place.
         * For outbound tunnels, the delivery process of these messages is
         *  initiated directly by the tunnel created (by sending to the first
         *  hop of the tunnel to create).
         * For inbound tunnels, the delivery process of these messages is
         *  initiated indirectly through an existing outbound tunnel of the
         *  creator.
         */
        class VariableTunnelBuild : public Message {
            public:
                /**
                 * Constructs from a std::list of pointers to i2pcpp::BuildRecord objects.
                 */
                VariableTunnelBuild(std::list<BuildRecordPtr> const &buildRecords);

                /**
                 * Constructs from a \a msgId and a std::list of pointers to
                 *  i2pcpp::BuildRecord objects.
                 */
                VariableTunnelBuild(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

                /**
                 * @return the std::list of pointers to i2pcpp::BuildRecord objects
                 */
                std::list<BuildRecordPtr> getRecords() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::VariableTunnelBuild
                 *  object.
                 * The format to be parsed is a 1B intger specifying the amount
                 *  of i2pcpp::BuildRecord objects, followed by that many 
                 *  i2pcpp::BuildRecord objects.
                 */
                static VariableTunnelBuild parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                VariableTunnelBuild() = default;

                /**
                 * Puts the 1B i2pcpp::BuildRecord count, followed by that many 
                 *  i2pcpp::BuildRecord objects in an i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                std::list<BuildRecordPtr> m_buildRecords;
        };
    }
}

#endif
