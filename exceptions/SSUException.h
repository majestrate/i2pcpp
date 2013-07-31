#ifndef SSUEXCEPTION_H
#define SSUEXCEPTION_H

#include <stdexcept>
#include <string>
#include "../datatypes/RouterHash.h"

namespace i2pcpp {

	class SSUException : public std::logic_error
	{
	public:
		SSUException() : std::logic_error("unknown error") {}
		SSUException(SSUException const & exc) : std::logic_error(exc) {}
		SSUException(std::string msg, RouterHash const & rh) : std::logic_error(std::string(msg + " " + std::string(rh))) {}
	};


}

#endif
