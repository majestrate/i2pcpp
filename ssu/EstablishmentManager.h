#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "../Thread.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterInfo.h"

#include "../util/LockingQueue.h"

#include "PacketBuilder.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class EstablishmentManager : public Thread {
			public:
				EstablishmentManager(UDPTransport &transport) : m_transport(transport) {}

				EstablishmentStatePtr getState(Endpoint const &ep);

				void establish(RouterInfo const &ri);
				inline void addWork(EstablishmentStatePtr const &es) { m_workQueue.enqueue(es); }

			private:
				void loop();
				void stopHook() { m_workQueue.finish(); }

				void sendRequest(EstablishmentStatePtr const &state);
				void processCreated(EstablishmentStatePtr const &state);
				void sendConfirmed(EstablishmentStatePtr const &state);

				UDPTransport& m_transport;
				PacketBuilder m_builder;

				LockingQueue<EstablishmentStatePtr> m_workQueue;

				std::unordered_map<Endpoint, EstablishmentStatePtr> m_stateTable;
				std::mutex m_stateTableMutex;
		};
	}
}

#endif
