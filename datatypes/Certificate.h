#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "Datatype.h"
#include "ByteArray.h"

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

			Certificate(ByteArray::const_iterator &certItr)
			{
				m_type = (Type)*(certItr++);
				if(m_type == NIL)
					return;
			}


			unsigned short getLength() const { return (1 + 2 + m_payload.size()); }

			ByteArray getBytes() const
			{
				ByteArray b(m_payload);
				unsigned short length = m_payload.size();

				b.insert(b.begin(), length);
				b.insert(b.begin(), (length >> 8));
				b.insert(b.begin(), (unsigned char)m_type);

				return b;
			}

		private:
			Type m_type;
			ByteArray m_payload;
	};
}

#endif
