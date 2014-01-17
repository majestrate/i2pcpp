/**
 * @file TunnelData.h
 * @brief Defines the i2pcpp::I2NP::TunnelData message type.
 */
#ifndef I2NPTUNNELDATA_H
#define I2NPTUNNELDATA_H

#include "../datatypes/StaticByteArray.h"

#include "Message.h"

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a tunnel data message. These are message sent by a tunnel
         *  gateway or participant to the next participant or endpoint.
         */
        class TunnelData : public Message {
            public:
                /**
                 * Constructs from a \a tunnelId and 1024 bytes of data.
                 * @param tunnelId identifies the tunnel that the message is
                 *  intended for
                 * @param data fragmented, batched, padded and encrypted I2NP messages
                 */
                TunnelData(uint32_t const tunnelId, StaticByteArray<1024> const &data);

                /**
                 * @return the tunnel identifier associated with this message
                 */
                uint32_t getTunnelId() const;

                /**
                 * @return the actual 1024 bytes of tunnel data
                 */
                const StaticByteArray<1024>& getData() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::TunnelData object.
                 * The format to be parsed is a 4B tunnel id and 1024B of data.
                 */
                static TunnelData parse(ByteArrayConstItr &begin, ByteArrayConstItr end);
            protected:
                TunnelData() = default;

                /**
                 * Puts the 4B tunnel identifier, followed by 1024B of data in
                 *  an i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                uint32_t m_tunnelId; ///< The 4 byte tunnel id
                StaticByteArray<1024> m_data; ///< 1024 bytes of data
        };
    }
}

#endif
