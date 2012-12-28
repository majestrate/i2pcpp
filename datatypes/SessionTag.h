#ifndef SESSIONTAG_H
#define SESSIONTAG_H

#include <array>

#include "Datatype.h"

namespace i2pcpp {
	class SessionTag : public Datatype {
		public:
			SessionTag();

			ByteArray getBytes() const;

		private:
			std::array<unsigned char, 32> m_tag;
	};
}

#endif
