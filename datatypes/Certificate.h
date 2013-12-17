/**
 * @file Certificate.h
 * @brief Defines the i2pcpp::Certificate structure.
 */
#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "Datatype.h"

namespace i2pcpp {

    /**
     * The certificate type. This is used for RIs, Garlic Cloves,
     *  Garlic Messages and Destinations.
     * Structure is:
     *  Type (1B) - Size (2B) - Payload (Variable)
     */
	class Certificate : public Datatype {
		public:
            /**
             * The enum for the certificate type field.
             */
			enum class Type {
				NIL,
				HASHCASH,
				HIDDEN,
				SIGNED,
				MULTIPLE
			};

			Certificate();

            /**
             * Constructs the certificate from an i2pcpp::ByteArray, given by an
             *  iterator to its begin and end.
             */
			Certificate(ByteArrayConstItr &begin, ByteArrayConstItr end);

            /**
             * @return the length of the certificates payload.
             */
			uint16_t getLength() const;

            /**
             * @return the serialized version of the certficate as an i2pcpp::ByteArray.
             */
			ByteArray serialize() const;

		private:
			Type m_type; ///< The certificate type
			ByteArray m_payload; ///< The actual certificate payload
	};
}

#endif
