#include "MessageReceiver.h"

#include "PacketHandler.h"

namespace i2pcpp {
	namespace SSU {
		void MessageReceiver::run()
		{
			while(m_handler.keepRunning()) {}
		}
	}
}
