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

				for(auto s: m_outboundTable) {
					s.second->lock();

					switch(s.second->getState()) {
						case OutboundEstablishmentState::INTRODUCED:
							cerr << "EstablishmentManager: sending session request to " << s.second->getEndpoint().toString() << "\n";
							sendRequest(s.second);
							break;

						case OutboundEstablishmentState::CREATED_RECEIVED:
							processCreated(s.second);
							break;

						case OutboundEstablishmentState::CONFIRMED_PARTIALLY:
							cerr << "EstablishmentManager: sending session confirmed to " << s.second->getEndpoint().toString() << "\n";
							sendConfirmed(s.second);
							break;
					}

					s.second->unlock();
				}

				m_outboundTableMutex.unlock();
			}
		}

		InboundEstablishmentStatePtr EstablishmentManager::getInboundState(Endpoint const &ep)
		{
			m_inboundTableMutex.lock();
			InboundEstablishmentStatePtr state = m_inboundTable[ep.toString()];
			m_inboundTableMutex.unlock();

			return state;
		}

		OutboundEstablishmentStatePtr EstablishmentManager::getOutboundState(Endpoint const &ep)
		{
			m_outboundTableMutex.lock();
			OutboundEstablishmentStatePtr state = m_outboundTable[ep.toString()];
			m_outboundTableMutex.unlock();

			return state;
		}

		void EstablishmentManager::establish(RouterInfo const &ri)
		{
			OutboundEstablishmentStatePtr oes(new OutboundEstablishmentState(m_transport.getContext(), ri));
			m_outboundTableMutex.lock();
			m_outboundTable[oes->getEndpoint().toString()] = oes;
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
		}
	}
}
