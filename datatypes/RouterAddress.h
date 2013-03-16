#ifndef ROUTERADDRESS_H
#define ROUTERADDRESS_H

#include <string>

#include "Datatype.h"
#include "Date.h"
#include "Mapping.h"

namespace i2pcpp {
	class RouterAddress : public Datatype {
		public:
			RouterAddress(int cost, Date const &expiration, std::string const &transport, Mapping const &options);
			RouterAddress(ByteArrayConstItr &begin, ByteArrayConstItr end);

			ByteArray serialize() const;
			const unsigned char getCost() const;
			const Date& getExpiration() const;
			const std::string& getTransport() const;
			const Mapping& getOptions() const;

		private:
			unsigned char m_cost;
			Date m_expiration;
			std::string m_transport;
			Mapping m_options;
	};
}

#endif
