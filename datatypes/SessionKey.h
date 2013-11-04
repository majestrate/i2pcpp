#ifndef SESSIONKEY_H
#define SESSIONKEY_H

#include "StaticByteArray.h"

namespace i2pcpp {
	typedef StaticByteArray<32, true> SessionKey;
}

#endif
