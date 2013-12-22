#include "EstablishmentManager.h"

#include <boost/bind.hpp>

#include "../../util/make_unique.h"

#include "../UDPTransport.h"

#include "PacketBuilder.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentManager::EstablishmentManager(UDPTransport &transport, Botan::DSA_PrivateKey const &privKey, RouterIdentity const &ri) :
			m_transport(transport),
			m_privKey(privKey),
			m_identity(ri),
			m_log(boost::log::keywords::channel = "EM") {}

		EstablishmentStatePtr EstablishmentManager::createState(Endpoint const &ep)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			auto es = std::make_shared<EstablishmentState>(m_privKey, m_identity, ep);
			m_stateTable[ep] = es;

			m_stateTimers[ep] = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 10));
			m_stateTimers[ep]->async_wait(boost::bind(&EstablishmentManager::timeoutCallback, this, boost::asio::placeholders::error, es));

			return es;
		}

		void EstablishmentManager::createState(Endpoint const &ep, RouterIdentity const &ri)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			auto es = std::make_shared<EstablishmentState>(m_privKey, m_identity, ep, ri);
			m_stateTable[ep] = es;

			sendRequest(es);

			m_stateTimers[ep] = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 10));
			m_stateTimers[ep]->async_wait(boost::bind(&EstablishmentManager::timeoutCallback, this, boost::asio::placeholders::error, es));
		}

		bool EstablishmentManager::stateExists(Endpoint const &ep) const
		{
			return (m_stateTable.count(ep) > 0);
		}

		void EstablishmentManager::post(EstablishmentStatePtr const &es)
		{
			m_transport.m_ios.post(boost::bind(&EstablishmentManager::stateChanged, this, es));
		}

		void EstablishmentManager::stateChanged(EstablishmentStatePtr es)
		{
			const Endpoint &ep = es->getTheirEndpoint();
			I2P_LOG_SCOPED_TAG(m_log, "Endpoint", ep);

			switch(es->getState())
			{
				case EstablishmentState::State::REQUEST_SENT:
					I2P_LOG(m_log, debug) << "sent session request";
					break;

				case EstablishmentState::State::REQUEST_RECEIVED:
					I2P_LOG(m_log, debug) << "received session request";
					processRequest(es);
					break;

				case EstablishmentState::State::CREATED_SENT:
					I2P_LOG(m_log, debug) << "sent session created";
					break;

				case EstablishmentState::State::CREATED_RECEIVED:
					I2P_LOG(m_log, debug) << "received session created";
					processCreated(es);
					break;

				case EstablishmentState::State::CONFIRMED_SENT:
					{
						const RouterHash &rh = es->getTheirIdentity().getHash();
						I2P_LOG_SCOPED_TAG(m_log, "RouterHash", rh);
						I2P_LOG(m_log, debug) << "sent session confirmed";
						m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_establishedSignal), rh, (es->getDirection() == EstablishmentState::Direction::INBOUND)));
						delState(ep);
					}
					break;

				case EstablishmentState::State::CONFIRMED_RECEIVED:
					I2P_LOG(m_log, debug) << "received session confirmed";
					processConfirmed(es);
					break;

				case EstablishmentState::State::UNKNOWN:
				case EstablishmentState::State::FAILURE:
					I2P_LOG(m_log, error) << "establishment failed";
					if(es->getDirection() == EstablishmentState::Direction::OUTBOUND)
						m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_failureSignal), es->getTheirIdentity().getHash()));

					delState(ep);
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

		void EstablishmentManager::delState(const Endpoint &ep)
		{
			std::lock_guard<std::mutex> lock(m_stateTableMutex);

			if(m_stateTimers[ep]) {
				m_stateTimers[ep]->cancel();
				m_stateTimers.erase(ep);
			}

			m_stateTable.erase(ep);
		}

		void EstablishmentManager::timeoutCallback(const boost::system::error_code& e, EstablishmentStatePtr es)
		{
			if(!e) {
				I2P_LOG_SCOPED_TAG(m_log, "Endpoint", es->getTheirEndpoint());
				I2P_LOG(m_log, debug) << "establishment timed out";

				es->setState(EstablishmentState::State::FAILURE);
				post(es);
			}
		}

		void EstablishmentManager::sendRequest(EstablishmentStatePtr const &state)
		{
			PacketPtr p = PacketBuilder::buildSessionRequest(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());

			m_transport.sendPacket(p);

			state->setState(EstablishmentState::State::REQUEST_SENT);
			post(state);
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

			m_transport.sendPacket(p);

			state->setState(EstablishmentState::State::CREATED_SENT);
			post(state);
		}

		void EstablishmentManager::processCreated(EstablishmentStatePtr const &state)
		{
			state->calculateDHSecret();

			if(!state->verifyCreationSignature()) {
				I2P_LOG(m_log, error) << "creation signature verification failed";
				state->setState(EstablishmentState::State::FAILURE);
				return;
			}

			const ByteArray& dhSecret = state->getDHSecret();
			SessionKey newKey(dhSecret), newMacKey;

			state->setSessionKey(newKey);

			copy(dhSecret.begin() + 32, dhSecret.begin() + 32 + 32, newMacKey.begin());
			state->setMacKey(newMacKey);

			Endpoint ep = state->getTheirEndpoint();
			PeerState ps(ep, state->getTheirIdentity().getHash());
			ps.setCurrentSessionKey(state->getSessionKey());
			ps.setCurrentMacKey(state->getMacKey());

			std::lock_guard<std::mutex> lock(m_transport.m_peers.getMutex());
			m_transport.m_peers.addPeer(std::move(ps));

			PacketPtr p = PacketBuilder::buildSessionConfirmed(state);
			p->encrypt(state->getSessionKey(), state->getMacKey());

			m_transport.sendPacket(p);

			state->setState(EstablishmentState::State::CONFIRMED_SENT);
			post(state);
		}

		void EstablishmentManager::processConfirmed(EstablishmentStatePtr const &state)
		{
			I2P_LOG_SCOPED_TAG(m_log, "RouterHash", state->getTheirIdentity().getHash());

			if(!state->verifyConfirmationSignature()) {
				I2P_LOG(m_log, error) << "confirmation signature verification failed";
				state->setState(EstablishmentState::State::FAILURE);
				post(state);

				return;
			} else
				I2P_LOG(m_log, debug) << "confirmation signature verification succeeded";

			Endpoint ep = state->getTheirEndpoint();
			PeerState ps(ep, state->getTheirIdentity().getHash());
			ps.setCurrentSessionKey(state->getSessionKey());
			ps.setCurrentMacKey(state->getMacKey());

			std::lock_guard<std::mutex> lock(m_transport.m_peers.getMutex());
			m_transport.m_peers.addPeer(std::move(ps));

			delState(ep);

			m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_establishedSignal), state->getTheirIdentity().getHash(), (state->getDirection() == EstablishmentState::Direction::INBOUND)));
		}
	}
}
