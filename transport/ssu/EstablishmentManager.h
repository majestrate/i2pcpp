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
				EstablishmentManager(UDPTransport &transport, Botan::DL_Group const &group, Botan::DSA_PrivateKey const &privKey);

				EstablishmentStatePtr createState(Endpoint const &ep, SessionKey const &sk);
				void createState(Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri);

				void post(EstablishmentStatePtr const &es);
				void stateChanged(EstablishmentStatePtr const &es);

			private:
				UDPTransport &m_transport;

				Botan::DL_Group m_group;
				Botan::DSA_PrivateKey m_privKey;

				std::unordered_map<Endpoint, EstablishmentStatePtr> m_stateTable;
				mutable std::mutex m_stateTableMutex;
		};
	}
}

#endif
