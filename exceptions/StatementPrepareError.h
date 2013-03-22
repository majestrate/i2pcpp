#ifndef STATEMENTPREPAREERROR_H
#define STATEMENTPREPAREERROR_H

#include <stdexcept>

namespace i2pcpp {
	class StatementPrepareError : public std::runtime_error {
		public:
			StatementPrepareError();
	};
}

#endif
