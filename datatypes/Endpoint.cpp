#include "Endpoint.h"

namespace i2pcpp {
	Endpoint::Endpoint(ByteArray const &addr, uint16_t port) : m_port(port)
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
}
