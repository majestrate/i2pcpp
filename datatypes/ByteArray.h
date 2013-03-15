#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <vector>

namespace i2pcpp {
	typedef std::vector<unsigned char> ByteArray;
	typedef ByteArray::iterator ByteArrayItr;
	typedef ByteArray::const_iterator ByteArrayConstItr;
}

#endif
