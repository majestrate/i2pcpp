#include "EstablishmentManager.h"

#include <boost/bind.hpp>

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentManager::EstablishmentManager(UDPTransport &transport, Botan::DL_Group const &group, Botan::DSA_PrivateKey const &privKey) :
			m_transport(transport),
			m_group(group),
			m_privKey(privKey) {}
	}
}
