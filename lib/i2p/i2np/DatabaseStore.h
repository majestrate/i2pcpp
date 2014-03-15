/**
 * @file DatabaseStore.h
 * @brief Defines the i2pcpp::I2NP::DatabaseStore message type.
 */
#ifndef I2NPDATABASESTORE_H
#define I2NPDATABASESTORE_H

#include "Message.h"

#include <i2pcpp/datatypes/RouterHash.h>
#include <i2pcpp/datatypes/ByteArray.h>

#include <array>

namespace i2pcpp {
    namespace I2NP {

        /**
         * Represents a database store message. This is used for either a
         *  database store operation request (send to a floodfill router), or
         *  as a response to a successful i2pcpp::DatabaseLookup message.
         * Contains either an uncompressed i2pcpp::LeaseSet or an compressed
         *  i2pcpp::RouterInfo.
         */
        class DatabaseStore : public Message {
            public:
                /**
                 * Specifies the type of data contained in the message:
                 * 0 indicates that it is a compressed i2pcpp::RouterInfo,
                 * 1 indicates that it is an uncompressed i2pcpp::LeaseSet.
                 */
                enum class DataType {
                    ROUTER_INFO = 0,
                    LEASE_SET = 1
                };

                /**
                 * Constructs from a 32B key, the I2NP::DatabaseStore::DataType,
                 *  a \a replyToken and an i2pcpp::ByteArray containing the RI
                 *  or LS.
                 * @param key the SHA256 hash of the RI or i2pcpp::Destination
                 * @param typ specifies the type of the data stored (RI or LS)
                 * @param replyToken if greater than 0, the receiver must reply
                 *  with an i2pcpp::I2NP::DeliveryStatus message; a floodfill
                 *  is then also required to flood the data to the closest
                 *  floodfill peers.
                 * @param data either an uncompressed LS, or a 2B integer (size)
                 *  followed by a compressed RI of \a size bytes
                 */
                DatabaseStore(StaticByteArray<32> const &key, DataType type, uint32_t replyToken, ByteArray const &data);

                /**
                 * @return the data type contained in this message (RI or LS)
                 */
                DataType getDataType() const;

                /**
                 * @return the reply token
                 */
                uint32_t getReplyToken() const;

                /**
                 * @return the underlying data
                 */
                const ByteArray& getData() const;

                /**
                 * Converts an i2pcpp::ByteArray to an i2pcpp::I2NP::DatabaseStore
                 *   object.
                 * The format to be parsed is a 32B SHA-256 hash as a key, followed by a
                 *  1B type identifer, followed a 4B reply token, followed by a 4B reply
                 *  tunnel identifer of the IBGW and its 32B i2pcpp::RouterHash. This is
                 *  followed by the actual data.
                 * @note reply ID and gateway are only included if reply token > 0
                 */
                static DatabaseStore parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

            protected:
                DatabaseStore() = default;

                /**
                 * Puts the 32B key, the 4B reply token and the data in an
                 *  i2pcpp::ByteArray.
                 */
                ByteArray compile() const;

            private:
                StaticByteArray<32> m_key;
                DataType m_type;
                uint32_t m_replyToken;
                uint32_t m_replyTunnelId;
                RouterHash m_replyGateway;
                ByteArray m_data;
        };
    }
}

#endif
