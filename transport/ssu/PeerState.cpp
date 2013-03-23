#include "PeerState.h"

#include <boost/bind.hpp>

namespace i2pcpp {
	namespace SSU {
		PeerState::PeerState(boost::asio::io_service &ios, Endpoint const &ep, RouterIdentity const &ri) :
			m_ios(ios),
			m_endpoint(ep),
			m_identity(ri) {}

		InboundMessageStatePtr PeerState::getInboundMessageState(const uint32_t msgId) const
		{
			InboundMessageStatePtr ims;

			auto itr = m_inboundMessageStates.find(msgId);
			if(itr != m_inboundMessageStates.end())
				ims = itr->second;

			return ims;
		}

		void PeerState::addInboundMessageState(InboundMessageStatePtr const &ims)
		{
			uint32_t msgId = ims->getMsgId();

			m_inboundMessageStates[msgId] = ims;

			std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 10)));

			timer->async_wait(boost::bind(&PeerState::inboundTimerCallback, this, boost::asio::placeholders::error, msgId));

			m_inboundTimers[msgId] = timer;
		}

		void PeerState::delInboundMessageState(const uint32_t msgId)
		{
			m_inboundMessageStates.erase(msgId);

			std::shared_ptr<boost::asio::deadline_timer> timer = m_inboundTimers[msgId];
			if(timer)
				timer->cancel();
		}

		void PeerState::delInboundMessageState(std::map<uint32_t, InboundMessageStatePtr>::const_iterator itr)
		{
			m_inboundMessageStates.erase(itr);
		}

		void PeerState::inboundTimerCallback(const boost::system::error_code& e, const uint32_t msgId)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if(!e) {
				// TODO Log when this happens?
				delInboundMessageState(msgId);
			}
		}

		SessionKey PeerState::getCurrentSessionKey() const
		{
			return m_sessionKey;
		}

		SessionKey PeerState::getCurrentMacKey() const
		{
			return m_macKey;
		}

		SessionKey PeerState::getNextSessionKey() const
		{
			return m_nextSessionKey;
		}

		SessionKey PeerState::getNextMacKey() const
		{
			return m_nextMacKey;
		}

		void PeerState::setCurrentSessionKey(SessionKey const &sk)
		{
			m_sessionKey = sk;
		}

		void PeerState::setCurrentMacKey(SessionKey const &mk)
		{
			m_macKey = mk;
		}

		void PeerState::setNextSessionKey(SessionKey const &sk)
		{
			m_nextSessionKey = sk;
		}

		void PeerState::setNextMacKey(SessionKey const &mk)
		{
			m_nextMacKey = mk;
		}

		const RouterIdentity& PeerState::getIdentity() const
		{
			return m_identity;
		}

		const Endpoint& PeerState::getEndpoint() const
		{
			return m_endpoint;
		}

		std::mutex& PeerState::getMutex() const
		{
			return m_mutex;
		}

		std::map<uint32_t, InboundMessageStatePtr>::iterator PeerState::begin()
		{
			return m_inboundMessageStates.begin();
		}

		std::map<uint32_t, InboundMessageStatePtr>::iterator PeerState::end()
		{
			return m_inboundMessageStates.end();
		}
	}
}
