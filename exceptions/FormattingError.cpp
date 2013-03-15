#include "FormattingError.h"

namespace i2pcpp {
	FormattingError::FormattingError() :
		std::runtime_error("Bad data") {}
}
