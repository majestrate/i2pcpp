#include "DeliveryStatus.h"

#include <iostream>

#include "../TestJob.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::MessageType DeliveryStatus::getType() const
		{
			return I2NP::Message::MessageType::DELIVERY_STATUS;
		}

		JobPtr DeliveryStatus::createJob() const
		{
			return JobPtr(new TestJob());
		}
	}
}
