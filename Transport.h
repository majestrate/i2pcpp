#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "datatypes/ByteArray.h"
#include "datatypes/RouterHash.h"

namespace i2pcpp {
	class RouterContext;

	class Transport {
		public:
			Transport(RouterContext& ctx) : m_ctx(ctx) {}

			virtual void connect(RouterHash const &rh) = 0;
			virtual void send(RouterHash const &rh, ByteArray const &data) = 0;
			virtual void disconnect(RouterHash const &rh) = 0;

		protected:
			RouterContext &m_ctx;
	};

	typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
