#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "../Thread.h"

#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterInfo.h"

#include "InboundEstablishmentState.h"
#include "OutboundEstablishmentState.h"

#include "PacketBuilder.h"

namespace i2pcpp {
	namespace SSU {
		class UDPTransport;

		class EstablishmentManager : public Thread {
			public:
				EstablishmentManager(UDPTransport &transport) : m_transport(transport) {}

				InboundEstablishmentStatePtr getInboundState(Endpoint const &ep);
				OutboundEstablishmentStatePtr getOutboundState(Endpoint const &ep);

				void establish(RouterInfo const &ri);

			private:
				void loop();

				void sendRequest(OutboundEstablishmentStatePtr const &state);
				void processCreated(OutboundEstablishmentStatePtr const &state);
				void sendConfirmed(OutboundEstablishmentStatePtr const &state);
				void processComplete(OutboundEstablishmentStatePtr const &state);

				UDPTransport& m_transport;
				PacketBuilder m_builder;

				std::unordered_map<Endpoint, InboundEstablishmentStatePtr> m_inboundTable;
				std::unordered_map<Endpoint, OutboundEstablishmentStatePtr> m_outboundTable;

				std::mutex m_inboundTableMutex;
				std::mutex m_outboundTableMutex;
		};
	}
}

#endif
