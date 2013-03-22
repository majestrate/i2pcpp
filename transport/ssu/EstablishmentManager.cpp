#include "EstablishmentManager.h"

#include <boost/bind.hpp>

#include "PacketBuilder.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentManager::EstablishmentManager(UDPTransport &transport, Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri) :
			m_transport(transport),
			m_privKey(privKey),
			m_identity(ri) {}

		EstablishmentStatePtr EstablishmentManager::createState(Endpoint const &ep, SessionKey const &sk)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_privKey, m_identity, ep));
			m_stateTable[ep] = es;

			return es;
		}

		void EstablishmentManager::createState(Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_privKey, m_identity, ep, ri));
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

		void EstablishmentManager::sendRequest(EstablishmentStatePtr const &state)
		{
			PacketPtr p = PacketBuilder::buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());
			state->setState(EstablishmentState::REQUEST_SENT);
			m_transport.sendPacket(p);
		}
	}
}
