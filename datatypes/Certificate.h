#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "Datatype.h"
#include "ByteArray.h"

#include <iostream>

namespace i2pcpp {
	class Certificate : public Datatype {
		public:
			enum Type {
				NIL,
				HASHCASH,
				HIDDEN,
				SIGNED,
				MULTIPLE
			};

			Certificate() : m_type(NIL) {}

			Certificate(ByteArray::const_iterator &certItr);

			unsigned short getLength() const { return (1 + 2 + m_payload.size()); }
			ByteArray getBytes() const;

		private:
			Type m_type;
			ByteArray m_payload;
	};
}

#endif
