#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <stdint.h>

#include <memory>
#include <vector>

namespace i2pcpp {
	typedef std::vector<unsigned char> ByteArray;
	typedef std::shared_ptr<ByteArray> ByteArrayPtr;
	typedef ByteArray::iterator ByteArrayItr;
	typedef ByteArray::const_iterator ByteArrayConstItr;
	std::ostream& operator<<(std::ostream &s, ByteArray const &data);
}


#endif
