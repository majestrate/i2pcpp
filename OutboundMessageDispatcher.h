#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

#include "Transport.h"

namespace i2pcpp {
	class OutboundMessageDispatcher {
		public:
			void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg) const;

			void registerTransport(TransportPtr const &t);

		private:
			TransportPtr m_transport;
	};
}

#endif
