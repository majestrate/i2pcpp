#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterInfo.h"

#include "InboundEstablishmentState.h"
#include "OutboundEstablishmentState.h"

#include "PacketBuilder.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class EstablishmentManager {
			public:
				EstablishmentManager(UDPTransport &transport) : m_transport(transport) {}

				void run();

				InboundEstablishmentStatePtr getInboundState(Endpoint const &ep);
				OutboundEstablishmentStatePtr getOutboundState(Endpoint const &ep);

				void establish(RouterInfo const &ri);

			private:
				void sendRequest(OutboundEstablishmentStatePtr const &state);
				void processCreated(OutboundEstablishmentStatePtr const &state);
				void sendConfirmed(OutboundEstablishmentStatePtr const &state);
				void processComplete(OutboundEstablishmentStatePtr const &state);

				UDPTransport& m_transport;
				PacketBuilder m_builder;

				unordered_map<Endpoint, InboundEstablishmentStatePtr> m_inboundTable;
				unordered_map<Endpoint, OutboundEstablishmentStatePtr> m_outboundTable;

				mutex m_inboundTableMutex;
				mutex m_outboundTableMutex;
		};
	}
}

#endif
