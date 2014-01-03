/**
 * @file VariableTunnelBuildReply.h
 * @brief Defines the i2pcpp::I2NP::VariableTunnelBuildReply message type.
 */
#ifndef I2NPVARIABLETUNNELBUILDREPLY_H
#define I2NPVARIABLETUNNELBUILDREPLY_H

#include <list>

#include "../datatypes/BuildRecord.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a response to an i2pcpp::I2NP::VariableTunnelBuild message.
         * For outbound tunnels, these messages are created by the OBEP and sent
         *  into the reply tunnel (an inbound tunnel specified in the
         *  i2pcpp::BuildRequestRecord object).
         * For inbound tunnels, this is not used, as the last hop of such a tunnel
         *  is the tunnel creator.
         */
        class VariableTunnelBuildReply : public Message {
            public:
                /**
                 * Constructs from a std::list of pointers to i2pcpp::BuildRecord objects.
                 */
                VariableTunnelBuildReply(std::list<BuildRecordPtr> const &buildRecords);

                /**
                 * Constructs from a \a msgId and a std::list of pointers to
                 *  i2pcpp::BuildRecord objects.
                 */
                VariableTunnelBuildReply(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

                /**
                 * @return the std::list of pointers to i2pcpp::BuildRecord objects
                 */
                std::list<BuildRecordPtr> getRecords() const;

                /**
                 * Converts an i2pcpp::ByteArray to an
                 *  i2pcpp::I2NP::VariableTunnelBuildReply object.
                 * The format to be parsed is a 1B intger specifying the amount
                 *  of i2pcpp::BuildRecord objects, followed by that many 
                 *  i2pcpp::BuildRecord objects.
                 */
                static VariableTunnelBuildReply parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                VariableTunnelBuildReply() = default;

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
