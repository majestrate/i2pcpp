#include "RecordNotFound.h"

namespace i2pcpp {
	RecordNotFound::RecordNotFound() :
		std::runtime_error("Record not found") {}
}
