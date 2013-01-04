#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

namespace i2pcpp {
	class RouterContext;

	class Transport {
		public:
			Transport(RouterContext& ctx) : m_ctx(ctx) {}

			virtual void connect(RouterHash const &rh) = 0;
			virtual void send(RouterHash const &rh, I2NP::MessagePtr const &msg) = 0;
			virtual void disconnect(RouterHash const &rh) = 0;

		protected:
			RouterContext &m_ctx;
	};

	typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
