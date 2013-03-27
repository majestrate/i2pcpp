#ifndef OUTBOUNDMESSAGEDISPATCHER_H
#define OUTBOUNDMESSAGEDISPATCHER_H

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"
#include "transport/Transport.h"

namespace i2pcpp {
	class OutboundMessageDispatcher {
		public:
			void sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg);
			void registerTransport(TransportPtr const &t);
			TransportPtr getTransport() const;

		private:
			TransportPtr m_transport;
	};
}

#endif
