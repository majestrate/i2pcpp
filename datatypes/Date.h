/**
 * @file Date.h
 * @brief Defines the i2pcpp::Date datatype.
 */
#ifndef DATE_H
#define DATE_H

#include "Datatype.h"

namespace i2pcpp {

    /**
     * Represents a data as a UNIX timestamp, that is, as the amount of seconds since
     * the unix epoch (00:00, January 1, 1970).
     */
	class Date : public Datatype {
		public:
            /**
             * Constructs from the current date.
             */
			Date();

            /**
             * Constructs a date from a given unix timestamp.
             * @param value amount of seconds from the unix epoch
             */
			Date(const uint64_t &value);

            /**
             * Constructs a date from iterators to the begin and end of an
             *  i2pcpp::ByteArray.
             * This i2pcpp::ByteArray should simply contain 8 bytes.
             * @throw i2pcpp::FormattingError
             */
			Date(ByteArrayConstItr &begin, ByteArrayConstItr end);

            /**
             * Serializes this i2pcpp::Data object as an i2pcpp::ByteArray of
             * 8 bytes.
             */
			ByteArray serialize() const;

		private:
            /// The underlying uint64 representing the timestamp
			uint64_t m_value;
	};
}

#endif
