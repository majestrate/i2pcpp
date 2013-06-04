#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <unordered_map>
#include <queue>

#include <botan/dsa.h>

#include "EstablishmentState.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class EstablishmentManager {
			public:
				EstablishmentManager(UDPTransport &transport, Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri);

				EstablishmentStatePtr createState(Endpoint const &ep);
				void createState(Endpoint const &ep, RouterIdentity const &ri);
				bool stateExists(Endpoint const &ep) const;

				void post(EstablishmentStatePtr const &es);
				void stateChanged(EstablishmentStatePtr es);
				EstablishmentStatePtr getState(Endpoint const &ep) const;

			private:
				void delState(const Endpoint &ep);
				void timeoutCallback(const boost::system::error_code& e, EstablishmentStatePtr es);

				void sendRequest(EstablishmentStatePtr const &state);
				void processRequest(EstablishmentStatePtr const &state);
				void processCreated(EstablishmentStatePtr const &state);
				void processConfirmed(EstablishmentStatePtr const &state);

				UDPTransport &m_transport;

				Botan::DSA_PrivateKey m_privKey;
				RouterIdentity m_identity;

				std::unordered_map<Endpoint, EstablishmentStatePtr> m_stateTable;
				std::unordered_map<Endpoint, std::shared_ptr<boost::asio::deadline_timer>> m_stateTimers;
				mutable std::mutex m_stateTableMutex;
		};
	}
}

#endif
