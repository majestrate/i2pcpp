#ifndef ESTABLISHMENTMANAGER_H
#define ESTABLISHMENTMANAGER_H

#include <unordered_map>
#include <queue>

#include <botan/dsa.h>

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class EstablishmentManager {
			public:
				EstablishmentManager(UDPTransport &transport, Botan::DL_Group const &group, Botan::DSA_PrivateKey const &privKey);

			private:
				UDPTransport &m_transport;

				Botan::DL_Group m_group;
				Botan::DSA_PrivateKey m_privKey;
		};
	}
}

#endif
