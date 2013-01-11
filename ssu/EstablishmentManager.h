#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <unordered_map>
#include <queue>

#include "../RouterContext.h"

#include "EstablishmentState.h"
#include "Packet.h"
#include "PeerStateList.h"
#include "PacketBuilder.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class EstablishmentManager : public boost::asio::io_service::service {
			public:
				EstablishmentManager(UDPTransport &transport);

				EstablishmentStatePtr getState(Endpoint const &ep) const;
				void establish(RouterInfo const &ri);
				void addWork(EstablishmentStatePtr const &es);

				void shutdown_service() {};

			private:
				void stateChanged(EstablishmentStatePtr const &es);

				void sendRequest(EstablishmentStatePtr const &state);
				void processCreated(EstablishmentStatePtr const &state);
				void sendConfirmed(EstablishmentStatePtr const &state);

				UDPTransport &m_transport;

				std::unordered_map<Endpoint, EstablishmentStatePtr> m_stateTable;
				mutable std::mutex m_stateTableMutex;
		};
	}
}

#endif
