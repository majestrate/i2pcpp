#include "DatabaseStore.h"

#include "../jobs/DatabaseStore.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DatabaseStore::getType() const
		{
			return I2NP::Message::Type::DB_STORE;
		}

		JobPtr DatabaseStore::createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const
		{
			return JobPtr(new Jobs::DatabaseStore(from, msg));
		}
	}
}
