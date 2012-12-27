#include "Message.h"

#include "DeliveryStatus.h"

#include <iostream>

using namespace std;

namespace i2pcpp {
	namespace I2NP {
		Message *Message::fromBytes(ByteArray const &data)
		{
			Message *m;

			auto dataItr = data.cbegin();
			MessageType mtype = (MessageType)*(dataItr++);

			switch(mtype)
			{
				case MessageType::DELIVERY_STATUS:
					m = new DeliveryStatus();
					break;

				default:
					return NULL;
			}

			return m;
		}
	}
}
