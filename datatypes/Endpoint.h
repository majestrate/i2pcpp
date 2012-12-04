#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#include "ByteArray.h"

using namespace boost::asio;
using namespace std;

namespace i2pcpp {
	class Endpoint {
		public:
			Endpoint() {}
			Endpoint(ip::udp::endpoint const &ep) : m_addr(ep.address()), m_port(ep.port()) {}
			Endpoint(string ip, unsigned short port) : m_addr(ip::address::from_string(ip)), m_port(port) {}
			Endpoint(ByteArray const &addr, unsigned short port) : m_port(port)
			{
				if(addr.size() == 4) {
					ip::address_v4::bytes_type b;
					copy(addr.begin(), addr.begin() + 4, b.begin());
					m_addr = ip::address_v4(b);
				}

				if(addr.size() == 16) {
					ip::address_v6::bytes_type b;
					copy(addr.begin(), addr.begin() + 16, b.begin());
					m_addr = ip::address_v6(b);
				}
			}

			ip::udp::endpoint getUDPEndpoint() const { return ip::udp::endpoint(m_addr, m_port); }
			string getIP() const { return m_addr.to_string(); }
			unsigned short getPort() const { return m_port; }
			string toString() const { return m_addr.to_string() + string(":") + boost::lexical_cast<string>(m_port); }
			ByteArray getRawIP() const
			{
				if(m_addr.is_v4()) {
					ip::address_v4::bytes_type b = m_addr.to_v4().to_bytes();
					ByteArray a(b.size());
					copy(b.begin(), b.end(), a.begin());
					return a;
				}

				if(m_addr.is_v6()) {
					ip::address_v6::bytes_type b = m_addr.to_v6().to_bytes();
					ByteArray a(b.size());
					copy(b.begin(), b.end(), a.begin());
					return a;
				}
			}

		private:
			ip::address m_addr;
			unsigned short m_port;
	};
}

#endif
