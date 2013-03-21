#include "EstablishmentManager.h"

#include <boost/bind.hpp>

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentManager::EstablishmentManager(UDPTransport &transport, Botan::DL_Group const &group, Botan::DSA_PrivateKey const &privKey) :
			m_transport(transport),
			m_group(group),
			m_privKey(privKey) {}

		EstablishmentStatePtr EstablishmentManager::createState(Endpoint const &ep, SessionKey const &sk)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_group, m_privKey, ep, sk));
			m_stateTable[ep] = es;

			return es;
		}

		void EstablishmentManager::createState(Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_group, m_privKey, ep, sk, ri));
			m_stateTable[ep] = es;

			post(es);
		}

		void EstablishmentManager::post(EstablishmentStatePtr const &es)
		{
			m_transport.post(boost::bind(&EstablishmentManager::stateChanged, this, es));
		}

		void EstablishmentManager::stateChanged(EstablishmentStatePtr const &es)
		{
		}
	}
}
