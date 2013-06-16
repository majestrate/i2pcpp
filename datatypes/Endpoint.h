#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>
#include <functional>

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#include "ByteArray.h"

namespace i2pcpp {
	class Endpoint {
		public:
			Endpoint(boost::asio::ip::udp::endpoint const &ep);
			Endpoint(std::string ip = "127.0.0.1", uint16_t port = 0);
			Endpoint(ByteArray const &addr, uint16_t port);

			boost::asio::ip::udp::endpoint getUDPEndpoint() const;
			ByteArray getRawIP() const;
			std::string getIP() const;
			uint16_t getPort() const;

			bool operator==(const Endpoint& rhs) const;

		private:
			boost::asio::ip::address m_addr;
			unsigned short m_port;
	};

	std::ostream& operator<<(std::ostream &s, Endpoint const &ep);
}

namespace std {
	template<>
	struct hash<i2pcpp::Endpoint> {
		public:
			size_t operator()(const i2pcpp::Endpoint &ep) const
			{
				hash<string> f;
				return f(ep.getIP() + ":" + boost::lexical_cast<std::string>(ep.getPort()));
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
