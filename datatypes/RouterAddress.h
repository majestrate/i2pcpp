/**
 * @file RouterAddress.h
 * @brief Defines the i2pcpp::RouterAddress datatype.
 */
#ifndef ROUTERADDRESS_H
#define ROUTERADDRESS_H

#include <string>

#include "Datatype.h"
#include "Date.h"
#include "Mapping.h"

namespace i2pcpp {

    /**
     * Datatype that allows a router to be contacted through a transport protocol.
     */
	class RouterAddress : public Datatype {
		public:
            /**
             * @param cost the weighed cost of using the address (0 = free, 255 = expensive)
             * @param expiration the expiration date of the address
             * @param transport the type of transport used as a string
             * @param options i2pcpp::Mapping of options related to transport
             * @warning expiration must be zero (currently unusued) 
             * @note cost is used for hashcash
             */
			RouterAddress(int cost, Date const &expiration, std::string const &transport, Mapping const &options);
            
            /**
             * Constructs from iterators to the begin and end of an i2pcpp::ByteArray.
             * Format must be:
             * cost (1B) | expiration (8B) | len (1B) | transport (len) | options
             */
			RouterAddress(ByteArrayConstItr &begin, ByteArrayConstItr end);

            /**
             * Serializes the object into an i2pcpp::ByteArray with the following
             * format:
             * cost (1B) | expiration (8B) | len (1B) | transport (len) | options
             */
			ByteArray serialize() const;

            /**
             * @return the cost of using the address (0 = free, 255 = expensive) 
             */
			const unsigned char getCost() const;

            /**
             * @return the expiration date
             * @warning curently unused
             */
			const Date& getExpiration() const;

            /**
             * @return the transport type as a string
             */
			const std::string& getTransport() const;

            /**
             * @return the i2pcpp::Mapping containing all the options related
             *  to the transport.
             */
			const Mapping& getOptions() const;

		private:
			unsigned char m_cost;
			Date m_expiration;
			std::string m_transport;
			Mapping m_options;
	};
}

#endif
