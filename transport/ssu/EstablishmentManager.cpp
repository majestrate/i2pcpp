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

		EstablishmentStatePtr EstablishmentManager::createState(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_privKey, m_identity, ep));
			m_stateTable[ep] = es;

			return es;
		}

		void EstablishmentManager::createState(Endpoint const &ep, RouterIdentity const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_privKey, m_identity, ep, ri));
			m_stateTable[ep] = es;

			sendRequest(es);

			post(es);
		}

		void EstablishmentManager::post(EstablishmentStatePtr const &es)
		{
			m_transport.post(boost::bind(&EstablishmentManager::stateChanged, this, es));
		}

		void EstablishmentManager::stateChanged(EstablishmentStatePtr const &es)
		{
			I2P_LOG_EP(m_transport.getLogger(), es->getTheirEndpoint());

			switch(es->getState())
			{
				case EstablishmentState::REQUEST_SENT:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "sent session request";
					break;

				case EstablishmentState::REQUEST_RECEIVED:
					BOOST_LOG_SEV(m_transport.getLogger(), debug) << "received session request";
					processRequest(es);
					break;
			}
		}

		void EstablishmentManager::sendRequest(EstablishmentStatePtr const &state)
		{
			PacketPtr p = PacketBuilder::buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());
			state->setState(EstablishmentState::REQUEST_SENT);
			m_transport.sendPacket(p);
		}

		void EstablishmentManager::processRequest(EstablishmentStatePtr const &state)
		{
			state->calculateDHSecret();

			PacketPtr p = PacketBuilder::buildSessionCreated(state);
			p->encrypt(state->getIV(), state->getSessionKey(), state->getMacKey());

			const ByteArray& dhSecret = state->getDHSecret();
			SessionKey newKey(dhSecret), newMacKey;

			state->setSessionKey(newKey);

			copy(dhSecret.begin() + 32, dhSecret.begin() + 32 + 32, newMacKey.begin());
			state->setMacKey(newMacKey);

			state->setState(EstablishmentState::CREATED_SENT);

			m_transport.sendPacket(p);
		}
	}
}
