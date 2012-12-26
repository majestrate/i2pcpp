#include "EstablishmentManager.h"

#include "UDPTransport.h"

#include "../datatypes/SessionKey.h"
#include "../util/Base64.h"
#include "../i2p.h"

#include <fstream>

namespace i2pcpp {
	namespace SSU {
		void EstablishmentManager::run()
		{
			while(m_transport.keepRunning()) {
				m_outboundTableMutex.lock();

				auto obtItr = m_outboundTable.begin();
				while(obtItr != m_outboundTable.end()) {
					OutboundEstablishmentStatePtr oes = (*obtItr).second;

					oes->lock();
					switch(oes->getState()) {
						case OutboundEstablishmentState::INTRODUCED:
							cerr << "EstablishmentManager: sending session request to " << oes->getEndpoint().toString() << "\n";
							sendRequest(oes);
							break;

						case OutboundEstablishmentState::CREATED_RECEIVED:
							processCreated(oes);
							break;

						case OutboundEstablishmentState::CONFIRMED_PARTIALLY:
							cerr << "EstablishmentManager: sending session confirmed to " << oes->getEndpoint().toString() << "\n";
							sendConfirmed(oes);
							break;

						case OutboundEstablishmentState::CONFIRMED_COMPLETELY:
							processComplete(oes);
							m_outboundTable.erase(obtItr++);
							oes->unlock();
							continue;
					}

					oes->unlock();
					++obtItr;
				}

				m_outboundTableMutex.unlock();
			}
		}

		InboundEstablishmentStatePtr EstablishmentManager::getInboundState(Endpoint const &ep)
		{
			InboundEstablishmentStatePtr ies;

			m_inboundTableMutex.lock();

			auto itr = m_inboundTable.find(ep);
			if(itr != m_inboundTable.end())
				ies = itr->second;

			m_inboundTableMutex.unlock();

			return ies;
		}

		OutboundEstablishmentStatePtr EstablishmentManager::getOutboundState(Endpoint const &ep)
		{
			OutboundEstablishmentStatePtr oes;

			m_outboundTableMutex.lock();

			auto itr = m_outboundTable.find(ep);
			if(itr != m_outboundTable.end())
				oes = itr->second;

			m_outboundTableMutex.unlock();

			return oes;
		}

		void EstablishmentManager::establish(RouterInfo const &ri)
		{
			OutboundEstablishmentStatePtr oes(new OutboundEstablishmentState(m_transport.getContext(), ri));
			m_outboundTableMutex.lock();
			m_outboundTable[oes->getEndpoint()] = oes;
			m_outboundTableMutex.unlock();
			oes->introduced();
		}

		void EstablishmentManager::sendRequest(OutboundEstablishmentStatePtr const &state)
		{
			PacketPtr p = m_builder.buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getSessionKey());
			state->requestSent();
			m_transport.send(p);
		}

		void EstablishmentManager::processCreated(OutboundEstablishmentStatePtr const &state)
		{
			state->calculateDHSecret();

			if(!state->verifyCreationSignature()) {
				cerr << "Signature verification failed!\n";
				state->validationFailed();
				return;
			}

			const ByteArray& dhSecret = state->getDHSecret();
			SessionKey newKey, newMacKey;

			copy(dhSecret.begin(), dhSecret.begin() + 32, newKey.begin());
			state->setSessionKey(newKey);

			copy(dhSecret.begin() + 32, dhSecret.begin() + 32 + 32, newMacKey.begin());
			state->setMacKey(newMacKey);

			state->confirmedPartially();
		}

		void EstablishmentManager::sendConfirmed(OutboundEstablishmentStatePtr const &state)
		{
			PacketPtr p = m_builder.buildSessionConfirmed(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());
			state->confirmedCompletely();
			m_transport.send(p);

			state->confirmedCompletely();
		}

		void EstablishmentManager::processComplete(OutboundEstablishmentStatePtr const &state)
		{
			Endpoint ep = state->getEndpoint();
			PeerStatePtr ps(new PeerState(ep, state->getIdentity(), false));
			ps->setCurrentSessionKey(state->getSessionKey());
			ps->setCurrentMacKey(state->getMacKey());
			m_transport.addRemotePeer(ps);
		}
	}
}
