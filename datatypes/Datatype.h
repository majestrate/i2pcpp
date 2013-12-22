/**
 * @file Datatype.h
 * @brief Defines the i2pcpp::Datatype interface.
 */
#ifndef DATATYPE_H
#define DATATYPE_H

#include "ByteArray.h"

namespace i2pcpp {

    /**
     * Interface for all datatype objects.
     */
	class Datatype {
		public:
			virtual ~Datatype() {};

            /**
             * Serializes this i2pcpp::Datatype into an i2pcpp::ByteArray.
             */
			virtual ByteArray serialize() const = 0;
	};
}

#endif
