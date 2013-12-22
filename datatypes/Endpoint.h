/**
 * @file Endpoint.h
 * @brief Defines the i2pcpp::Endpoint type.
 */
#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>
#include <functional>

#include <boost/asio.hpp>

#include "ByteArray.h"

namespace i2pcpp {
    /**
     * Represents any UDP endpoint.
     */
    class Endpoint {
        public:
            Endpoint(boost::asio::ip::udp::endpoint const &ep);
            Endpoint(std::string ip = "127.0.0.1", uint16_t port = 0);
            Endpoint(ByteArray const &addr, uint16_t port);

            boost::asio::ip::udp::endpoint getUDPEndpoint() const;

            /**
             * @return the IP address as a byte array
             */
            ByteArray getRawIP() const;

            /**
             * @return the IP address in human-readable notation
             */
            std::string getIP() const;

            /**
             * @return the port for the endpoint
             */
            uint16_t getPort() const;

            /**
             * Compares this endpoint with another endpoint by comparing their
             * addresses and ports. Two endpoints are said to be equal if their ports
             * and addresses are equal.
             * @return true if they equal, false otherwise
             */
            bool operator==(const Endpoint& rhs) const;

            /**
             * Creates a std::string representation of the endpoint. The format is
             *  IP address + ":" + port.
             * @return the string representation of the endpoint
             */
            operator std::string() const;

        private:
            boost::asio::ip::address m_addr;
            unsigned short m_port;
    };

    std::ostream& operator<<(std::ostream &s, Endpoint const &ep);
    std::size_t hash_value(Endpoint const &ep);
}

namespace std {
    template<>
    struct hash<i2pcpp::Endpoint> {
        public:
            size_t operator()(const i2pcpp::Endpoint &ep) const
            {
                hash<string> f;
                return f(ep);
            }
    };

    template<>
    struct equal_to<i2pcpp::Endpoint> {
        public:
            bool operator()(const i2pcpp::Endpoint& lhs, const i2pcpp::Endpoint& rhs) const
            {
                return lhs == rhs;
            }
    };
}

#endif
