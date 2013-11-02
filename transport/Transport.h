#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <boost/signals2.hpp>

#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterHash.h"
#include "../datatypes/RouterInfo.h"

namespace i2pcpp {
	class Transport {
		public:
			typedef boost::signals2::signal<void(const RouterHash, bool)> EstablishedSignal;
			typedef boost::signals2::signal<void(const RouterHash, const uint32_t, const ByteArray)> ReceivedSignal;
			typedef boost::signals2::signal<void(const RouterHash)> FailureSignal;
			typedef boost::signals2::signal<void(const RouterHash)> DisconnectedSignal;

			Transport();
			Transport(const Transport &) = delete;
			Transport& operator=(Transport &) = delete;
			virtual ~Transport();

			virtual void connect(RouterInfo const &ri) = 0;
			virtual void send(RouterHash const &rh, uint32_t msgId, ByteArray const &msg) = 0;
			virtual void disconnect(RouterHash const &rh) = 0;
			virtual uint32_t numPeers() const = 0;
			virtual bool isConnected(RouterHash const &rh) const = 0;

			boost::signals2::connection registerEstablishedHandler(EstablishedSignal::slot_type const &eh);
			boost::signals2::connection registerReceivedHandler(ReceivedSignal::slot_type const &rh);
			boost::signals2::connection registerFailureSignal(FailureSignal::slot_type const &fs);
			boost::signals2::connection registerDisconnectedSignal(DisconnectedSignal::slot_type const &ds);

		protected:
			EstablishedSignal m_establishedSignal;
			ReceivedSignal m_receivedSignal;
			FailureSignal m_failureSignal;
			DisconnectedSignal m_disconnectedSignal;
	};

	typedef std::shared_ptr<Transport> TransportPtr;
}

#endif
