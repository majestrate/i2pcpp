#include "DeliveryStatus.h"

#include <iostream>

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DeliveryStatus::getType() const
		{
			return I2NP::Message::Type::DELIVERY_STATUS;
		}

		JobPtr DeliveryStatus::createJob(RouterHash const &from, I2NP::MessagePtr const &msg) const
		{
			return JobPtr();
		}
	}
}
