#ifndef DATE_H
#define DATE_H

#include <chrono>

#include "Datatype.h"

using namespace std;

namespace i2pcpp {
	class Date : public Datatype {
		public:
			Date();
			Date(ByteArray::const_iterator &dateItr);

			ByteArray getBytes() const;

		private:
			unsigned long long m_value;
	};
}

#endif
