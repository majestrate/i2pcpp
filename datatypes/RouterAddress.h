#ifndef ROUTERADDRESS_H
#define ROUTERADDRESS_H

#include <string>

#include "Datatype.h"
#include "Date.h"
#include "Mapping.h"

namespace i2pcpp {
	class RouterAddress : public Datatype {
		public:
			RouterAddress(int cost, Date const &expiration, std::string const &transport, Mapping const &options) : m_cost(cost), m_expiration(expiration), m_transport(transport), m_options(options) {}

			RouterAddress(ByteArray::const_iterator &addrItr)
			{
				m_cost = *(addrItr++);
				m_expiration = Date(addrItr);
				m_options = Mapping(addrItr);
			}

			ByteArray getBytes() const
			{
				ByteArray b;

				ByteArray expiration = m_expiration.getBytes();
				ByteArray options = m_options.getBytes();

				b.insert(b.end(), m_cost);
				b.insert(b.end(), expiration.begin(), expiration.end());
				b.insert(b.end(), m_transport.size());
				b.insert(b.end(), m_transport.begin(), m_transport.end());
				b.insert(b.end(), options.begin(), options.end());

				return b;
			}

			std::string getHost() const
			{
				return m_options.getValue("host");
			}

			unsigned short getPort() const
			{
				return stoi(m_options.getValue("port"));
			}

		private:
			unsigned char m_cost;
			Date m_expiration;
			std::string m_transport;
			Mapping m_options;
	};
}

#endif
