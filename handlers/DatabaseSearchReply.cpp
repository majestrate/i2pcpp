#include "DatabaseSearchReply.h"

#include "../jobs/DatabaseSearchReply.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DatabaseSearchReply::getType() const
		{
			return I2NP::Message::Type::DB_SEARCH_REPLY;
		}

		JobPtr DatabaseSearchReply::createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const
		{
			return JobPtr(new Jobs::DatabaseSearchReply(m_ctx, from, msg));
		}
	}
}
