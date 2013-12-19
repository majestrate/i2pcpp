/**
 * @file SessionKey.h
 * @brief Defines the i2pcpp::SessionKey typedef.
 */
#ifndef SESSIONKEY_H
#define SESSIONKEY_H

#include "StaticByteArray.h"

namespace i2pcpp {
	typedef StaticByteArray<32> SessionKey;
}

#endif
