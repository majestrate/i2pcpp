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
			RouterAddress(ByteArray::const_iterator &addrItr);

			ByteArray getBytes() const;
			const unsigned char getCost() const { return m_cost; }
			const Date& getExpiration() const { return m_expiration; }
			const std::string& getTransport() const { return m_transport; }
			const Mapping& getOptions() const { return m_options; }

		private:
			unsigned char m_cost;
			Date m_expiration;
			std::string m_transport;
			Mapping m_options;
	};
}

#endif
