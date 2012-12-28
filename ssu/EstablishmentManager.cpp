#include "EstablishmentManager.h"

#include "UDPTransport.h"

#include "../datatypes/SessionKey.h"
#include "../util/Base64.h"

#include <fstream>

namespace i2pcpp {
	namespace SSU {
		void EstablishmentManager::loop()
		{
			while(m_keepRunning) {
				std::lock_guard<std::mutex> lock(m_outboundTableMutex);

				auto obtItr = m_outboundTable.begin();
				while(obtItr != m_outboundTable.end()) {
					OutboundEstablishmentStatePtr oes = (*obtItr).second;

					std::lock_guard<std::mutex> lock(oes->getMutex());

					switch(oes->getState()) {
						case OutboundEstablishmentState::INTRODUCED:
							std::cerr << "EstablishmentManager: sending session request to " << oes->getEndpoint().toString() << "\n";
							sendRequest(oes);
							break;

						case OutboundEstablishmentState::CREATED_RECEIVED:
							processCreated(oes);
							break;

						case OutboundEstablishmentState::CONFIRMED_PARTIALLY:
							std::cerr << "EstablishmentManager: sending session confirmed to " << oes->getEndpoint().toString() << "\n";
							sendConfirmed(oes);
							break;

						case OutboundEstablishmentState::CONFIRMED_COMPLETELY:
							processComplete(oes);
							m_outboundTable.erase(obtItr++);
							continue;
					}

					++obtItr;
				}
			}
		}

		InboundEstablishmentStatePtr EstablishmentManager::getInboundState(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_inboundTableMutex);

			InboundEstablishmentStatePtr ies;

			auto itr = m_inboundTable.find(ep);
			if(itr != m_inboundTable.end())
				ies = itr->second;

			return ies;
		}

		OutboundEstablishmentStatePtr EstablishmentManager::getOutboundState(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_outboundTableMutex);

			OutboundEstablishmentStatePtr oes;

			auto itr = m_outboundTable.find(ep);
			if(itr != m_outboundTable.end())
				oes = itr->second;

			return oes;
		}

		void EstablishmentManager::establish(RouterInfo const &ri)
		{
			std::lock_guard<std::mutex> lock(m_outboundTableMutex);

			OutboundEstablishmentStatePtr oes(new OutboundEstablishmentState(m_transport.getContext(), ri));
			m_outboundTable[oes->getEndpoint()] = oes;
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
				std::cerr << "Signature verification failed!\n";
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
