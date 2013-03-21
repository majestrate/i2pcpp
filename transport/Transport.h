#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <boost/signals2.hpp>

#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterHash.h"
#include "../datatypes/RouterInfo.h"

namespace i2pcpp {
	class Transport {
		public:
			typedef boost::signals2::signal<void(const RouterHash&)> EstablishedSignal;
			typedef boost::signals2::signal<void(const RouterHash&, const ByteArray&)> ReceivedSignal;

			virtual void connect(RouterInfo const &ri) = 0;
			virtual void send(RouterHash const &rh, ByteArray const &msg) = 0;
			virtual void disconnect(RouterHash const &rh) = 0;

			boost::signals2::connection registerEstablishedHandler(EstablishedSignal::slot_type const &eh);
			boost::signals2::connection registerReceivedHandler(ReceivedSignal::slot_type const &rh);

		protected:
			EstablishedSignal m_establishedSignal;
			ReceivedSignal m_receivedSignal;
	};

	typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
