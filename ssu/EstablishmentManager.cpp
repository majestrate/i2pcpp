#include "EstablishmentManager.h"

#include "UDPTransport.h"

#include "../datatypes/SessionKey.h"
#include "../util/Base64.h"

#include <thread>

namespace i2pcpp {
	namespace SSU {
		void EstablishmentManager::loop()
		{
			while(m_keepRunning) {
				m_workQueue.wait();
				EstablishmentStatePtr es = m_workQueue.pop();

				if(!es)
					continue;

				std::lock_guard<std::mutex> lock(es->getMutex());

				switch(es->getState()) {
					case EstablishmentState::INTRODUCED:
						std::cerr << "EstablishmentManager: sending session request to " << es->getTheirEndpoint().toString() << "\n";
						sendRequest(es);
						break;

					case EstablishmentState::CREATED_RECEIVED:
						processCreated(es);
						addWork(es);
						break;

					case EstablishmentState::CONFIRMED_PARTIALLY:
						std::cerr << "EstablishmentManager: sending session confirmed to " << es->getTheirEndpoint().toString() << "\n";
						sendConfirmed(es);
						addWork(es);
						break;

					case EstablishmentState::CONFIRMED_COMPLETELY:
						m_stateTableMutex.lock();
						m_stateTable.erase(es->getTheirEndpoint());
						m_stateTableMutex.unlock();
						break;
				}
			}
		}

		EstablishmentStatePtr EstablishmentManager::getState(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es;

			auto itr = m_stateTable.find(ep);
			if(itr != m_stateTable.end())
				es = itr->second;

			return es;
		}

		void EstablishmentManager::establish(RouterInfo const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			EstablishmentStatePtr es(new EstablishmentState(m_transport.m_ctx, ri, false));
			m_stateTable[es->getTheirEndpoint()] = es;
			es->introduced();
			addWork(es);
		}

		void EstablishmentManager::sendRequest(EstablishmentStatePtr const &state)
		{
			PacketPtr p = m_builder.buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getSessionKey());
			state->requestSent();
			m_transport.m_outboundQueue.enqueue(p);
		}

		void EstablishmentManager::processCreated(EstablishmentStatePtr const &state)
		{
			state->calculateDHSecret();

			if(!state->verifyCreationSignature()) {
				std::cerr << "Signature verification failed!\n";
				state->validationFailed();
				return;
			}

			const ByteArray& dhSecret = state->getDHSecret();
			SessionKey newKey(dhSecret), newMacKey;

			state->setSessionKey(newKey);

			copy(dhSecret.begin() + 32, dhSecret.begin() + 32 + 32, newMacKey.begin());
			state->setMacKey(newMacKey);

			state->confirmedPartially();
		}

		void EstablishmentManager::sendConfirmed(EstablishmentStatePtr const &state)
		{
			Endpoint ep = state->getTheirEndpoint();
			PeerStatePtr ps(new PeerState(ep, state->getIdentity(), false));
			ps->setCurrentSessionKey(state->getSessionKey());
			ps->setCurrentMacKey(state->getMacKey());
			m_transport.addRemotePeer(ps);

			PacketPtr p = m_builder.buildSessionConfirmed(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());
			m_transport.m_outboundQueue.enqueue(p);

			state->confirmedCompletely();
		}
	}
}
