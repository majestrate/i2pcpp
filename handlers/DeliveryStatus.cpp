#include "DeliveryStatus.h"

#include <iostream>

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DeliveryStatus::getType() const
		{
			return I2NP::Message::Type::DELIVERY_STATUS;
		}

		void DeliveryStatus::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
		}
	}
}
