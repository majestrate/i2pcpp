#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <boost/signals2.hpp>

#include "datatypes/RouterHash.h"
#include "i2np/Message.h"

namespace i2pcpp {
	class RouterContext;

	class Transport {
		public:
			typedef boost::signals2::signal<void(const RouterHash&)> EstablishedSignal;
			typedef boost::signals2::signal<void(const RouterHash&, const ByteArray&)> ReceivedSignal;

			Transport(RouterContext& ctx) : m_ctx(ctx) {}

			virtual void connect(RouterHash const &rh) = 0;
			virtual void send(RouterHash const &rh, I2NP::MessagePtr const &msg) = 0;
			virtual void disconnect(RouterHash const &rh) = 0;

			boost::signals2::connection registerEstablishedHandler(EstablishedSignal::slot_type const &eh);
			boost::signals2::connection registerReceivedHandler(ReceivedSignal::slot_type const &rh);

		protected:
			RouterContext &m_ctx;

			EstablishedSignal m_establishedSignal;
			ReceivedSignal m_receivedSignal;
	};

	typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
