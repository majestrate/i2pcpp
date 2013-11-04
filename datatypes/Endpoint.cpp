#include "Endpoint.h"

namespace i2pcpp {
	Endpoint::Endpoint(boost::asio::ip::udp::endpoint const &ep) :
		m_addr(ep.address()),
		m_port(ep.port()) {}

	Endpoint::Endpoint(std::string ip, uint16_t port) :
		m_addr(boost::asio::ip::address::from_string(ip)),
		m_port(port) {}

	Endpoint::Endpoint(ByteArray const &addr, uint16_t port) :
		m_port(port)
	{
		if(addr.size() == 4) {
			boost::asio::ip::address_v4::bytes_type b;
			copy(addr.begin(), addr.begin() + 4, b.begin());
			m_addr = boost::asio::ip::address_v4(b);
		}

		if(addr.size() == 16) {
			boost::asio::ip::address_v6::bytes_type b;
			copy(addr.begin(), addr.begin() + 16, b.begin());
			m_addr = boost::asio::ip::address_v6(b);
		}
	}

	boost::asio::ip::udp::endpoint Endpoint::getUDPEndpoint() const
	{
		return boost::asio::ip::udp::endpoint(m_addr, m_port);
	}

	ByteArray Endpoint::getRawIP() const
	{
		if(m_addr.is_v4()) {
			boost::asio::ip::address_v4::bytes_type b = m_addr.to_v4().to_bytes();
			ByteArray a(b.size());
			copy(b.begin(), b.end(), a.begin());
			return a;
		} else {
			boost::asio::ip::address_v6::bytes_type b = m_addr.to_v6().to_bytes();
			ByteArray a(b.size());
			copy(b.begin(), b.end(), a.begin());
			return a;
		}
	}

	std::string Endpoint::getIP() const
	{
		return m_addr.to_string();
	}

	uint16_t Endpoint::getPort() const
	{
		return m_port;
	}

	bool Endpoint::operator==(const Endpoint& rhs) const
	{
		return m_addr == rhs.m_addr && m_port == rhs.m_port;
	}

	Endpoint::operator std::string() const
	{
		return m_addr.to_string() + ":" + boost::lexical_cast<std::string>(m_port);
	}

	std::ostream& operator<<(std::ostream &s, Endpoint const &ep)
	{
		s << std::string(ep);
		return s;
	}
}
