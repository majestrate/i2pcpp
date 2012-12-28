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
			Endpoint() {}
			Endpoint(boost::asio::ip::udp::endpoint const &ep) : m_addr(ep.address()), m_port(ep.port()) {}
			Endpoint(std::string ip, unsigned short port) : m_addr(boost::asio::ip::address::from_string(ip)), m_port(port) {}
			Endpoint(ByteArray const &addr, unsigned short port);

			boost::asio::ip::udp::endpoint getUDPEndpoint() const { return boost::asio::ip::udp::endpoint(m_addr, m_port); }
			std::string getIP() const { return m_addr.to_string(); }
			unsigned short getPort() const { return m_port; }
			std::string toString() const
			{
				return m_addr.to_string() + std::string(":") + boost::lexical_cast<std::string>(m_port);
			}

			ByteArray getRawIP() const;

			bool operator==(const Endpoint& rhs) const
			{
				return m_addr == rhs.m_addr && m_port == rhs.m_port;
			}

		private:
			boost::asio::ip::address m_addr;
			unsigned short m_port;
	};
}

template<>
struct std::hash<i2pcpp::Endpoint> {
	public:
		size_t operator()(const i2pcpp::Endpoint &ep) const
		{
			std::hash<string> f;
			return f(ep.toString());
		}
};

template<>
struct std::equal_to<i2pcpp::Endpoint> {
	public:
		bool operator()(const i2pcpp::Endpoint& lhs, const i2pcpp::Endpoint& rhs) const
		{
			return lhs == rhs;
		}
};

#endif
