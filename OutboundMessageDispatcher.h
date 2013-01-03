#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

#include "Transport.h"

namespace i2pcpp {
	class OutboundMessageDispatcher {
		public:
			OutboundMessageDispatcher(TransportPtr const &transport) : m_transport(transport) {}

			void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg) const;

		private:
			TransportPtr m_transport;
	};
}

#endif
