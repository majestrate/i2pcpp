#include "EstablishmentManager.h"

#include <boost/bind.hpp>

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentManager::EstablishmentManager(UDPTransport &transport) :
			m_transport(transport) {}

		void EstablishmentManager::stateChanged(EstablishmentStatePtr const &es)
		{
			switch(es->getState()) {
				case EstablishmentState::INTRODUCED:
					std::cerr << "EstablishmentManager: sending session request to " << es->getTheirEndpoint().toString() << "\n";
					sendRequest(es);
					break;

				case EstablishmentState::REQUEST_RECEIVED:
					std::cerr << "EstablishmentManager: sending session created to " << es->getTheirEndpoint().toString() << "\n";
					sendCreated(es);
					break;

				case EstablishmentState::CREATED_SENT:
					break;

				case EstablishmentState::CREATED_RECEIVED:
					processCreated(es);
					break;

				case EstablishmentState::CONFIRMED_PARTIALLY:
					std::cerr << "EstablishmentManager: sending session confirmed to " << es->getTheirEndpoint().toString() << "\n";
					sendConfirmed(es);
					break;

				case EstablishmentState::CONFIRMED_COMPLETELY:
					m_stateTableMutex.lock();
					m_stateTable.erase(es->getTheirEndpoint());
					m_stateTableMutex.unlock();
					m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_establishedSignal), es->getTheirIdentity().getHash()));
					break;
			}
		}

		EstablishmentStatePtr EstablishmentManager::getState(Endpoint const &ep) const
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es;

			auto itr = m_stateTable.find(ep);
			if(itr != m_stateTable.end())
				es = itr->second;

			return es;
		}

		EstablishmentStatePtr EstablishmentManager::establish(Endpoint const &ep, SessionKey const &sk)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_transport.m_ctx, ep, sk));
			m_stateTable[ep] = es;

			return es;
		}

		void EstablishmentManager::establish(Endpoint const &ep, SessionKey const &sk, RouterIdentity const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_transport.m_ctx, ep, sk, ri));
			m_stateTable[ep] = es;

			es->introduced();

			addWork(es);
		}

		void EstablishmentManager::addWork(EstablishmentStatePtr const &es)
		{
			m_transport.m_ios.post(boost::bind(&EstablishmentManager::stateChanged, this, es));
		}

		void EstablishmentManager::sendRequest(EstablishmentStatePtr const &state)
		{
			PacketPtr p = PacketBuilder::buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getSessionKey());
			state->requestSent();
			m_transport.sendPacket(p);
		}

		void EstablishmentManager::sendCreated(EstablishmentStatePtr const &state)
		{
			PacketPtr p = PacketBuilder::buildSessionCreated(state);
			p->encrypt(state->getSessionKey(), state->getSessionKey());
			state->createdSent();
			m_transport.sendPacket(p);
		}

		void EstablishmentManager::processCreated(EstablishmentStatePtr const &state)
		{
			state->calculateDHSecret();

			if(!state->verifyCreationSignature()) {
				std::cerr << "EstablishmentManager: Signature verification failed!\n";
				state->validationFailed();
				return;
			}

			const ByteArray& dhSecret = state->getDHSecret();
			SessionKey newKey(dhSecret), newMacKey;

			state->setSessionKey(newKey);

			copy(dhSecret.begin() + 32, dhSecret.begin() + 32 + 32, newMacKey.begin());
			state->setMacKey(newMacKey);

			state->confirmedPartially();
			addWork(state);
		}

		void EstablishmentManager::sendConfirmed(EstablishmentStatePtr const &state)
		{
			Endpoint ep = state->getTheirEndpoint();
			PeerStatePtr ps(new PeerState(ep, state->getTheirIdentity(), false));
			ps->setCurrentSessionKey(state->getSessionKey());
			ps->setCurrentMacKey(state->getMacKey());
			m_transport.m_peers.addRemotePeer(ps);

			PacketPtr p = PacketBuilder::buildSessionConfirmed(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());
			m_transport.sendPacket(p);

			state->confirmedCompletely();
			addWork(state);
		}
	}
}
