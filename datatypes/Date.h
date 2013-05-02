#ifndef DATE_H
#define DATE_H
#include <cstdint>

#include "Datatype.h"

namespace i2pcpp {
	class Date : public Datatype {
		public:
			Date();
			Date(const uint64_t &value);
			Date(ByteArrayConstItr &begin, ByteArrayConstItr end);

			ByteArray serialize() const;

		private:
			uint64_t m_value;
	};
}

#endif
