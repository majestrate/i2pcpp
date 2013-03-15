#ifndef DATATYPE_H
#define DATATYPE_H

#include "ByteArray.h"

namespace i2pcpp {
	class Datatype {
		public:
			Datatype();
			virtual ByteArray serialize() const = 0;
	};
}

#endif
