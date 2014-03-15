/**
 * @file TunnelGateway.h
 * @brief Defines the i2pcpp::I2NP::TunnelGateway message type.
 */
#ifndef I2NPTUNNELGATEWAY_H
#define I2NPTUNNELGATEWAY_H

#include "Message.h"

#include <list>

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a tunnel gateway message. These messages wrap another
         *  i2pcpp::I2NP::Message to be sent into the tunnel, at the tunnel
         *  inbound gateway.
         */
        class TunnelGateway : public Message {
            public:
                /**
                 * Constructs from a \a tunnelId and a variable amount of data.
                 * @param tunnelId identifies the tunnel that the message is
                 *  intended for
                 * @param data an I2NP message with a standard header
                 */
                TunnelGateway(uint32_t const tunnelId, ByteArray const &data);

                /**
                 * @return the tunnel identifier associated with this message
                 */
                uint32_t getTunnelId() const;

                /**
                 * @return the data
                 */
                const ByteArray& getData() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::TunnelData object.
                 * The format to be parsed is a 4B tunnel id, followed by a 2B length,
                 *  and that amount bytes of data.
                 */
                static TunnelGateway parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                TunnelGateway() = default;


                /**
                 * Puts the 4B tunnel identifier, followed by the 2B length,
                 * followed by that amount of bytes of data in an i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                uint32_t m_tunnelId; ///< The 4 byte tunnel id
                ByteArray m_data; ///< the variable amount of data
        };
    }
}

#endif
