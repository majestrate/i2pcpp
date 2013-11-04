#include "PeerState.h"

#include <boost/bind.hpp>

#include "../../Log.h"

namespace i2pcpp {
	namespace SSU {
		PeerState::PeerState(boost::asio::io_service &ios, Endpoint const &ep, RouterIdentity const &ri) :
			m_ios(ios),
			m_endpoint(ep),
			m_identity(ri),
	 		m_log(boost::log::keywords::channel = "PS")
		{
			I2P_LOG_TAG(m_log, "RouterHash", m_identity.getHash());
		}

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
			std::shared_ptr<boost::asio::deadline_timer> timer = m_inboundTimers[msgId];
			if(timer) {
				I2P_LOG(m_log, debug) << "canceling IMS timer";
				timer->cancel();
				m_inboundTimers.erase(msgId);
			}

			m_inboundMessageStates.erase(msgId);
		}

		void PeerState::delInboundMessageState(std::map<uint32_t, InboundMessageStatePtr>::const_iterator itr)
		{
			uint32_t msgId = itr->second->getMsgId();
			std::shared_ptr<boost::asio::deadline_timer> timer = m_inboundTimers[msgId];
			if(timer) {
				I2P_LOG(m_log, debug) << "canceling IMS timer";
				timer->cancel();
				m_inboundTimers.erase(msgId);
			}

			m_inboundMessageStates.erase(itr);
		}

		void PeerState::inboundTimerCallback(const boost::system::error_code& e, const uint32_t msgId)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if(!e) {
				I2P_LOG(m_log, debug) << "removing IMS due to timeout";
				m_inboundMessageStates.erase(msgId);
				m_inboundTimers.erase(msgId);
			}
		}

		OutboundMessageStatePtr PeerState::getOutboundMessageState(const uint32_t msgId) const
		{
			OutboundMessageStatePtr oms;

			auto itr = m_outboundMessageStates.find(msgId);
			if(itr != m_outboundMessageStates.end())
				oms = itr->second;

			return oms;
		}

		void PeerState::addOutboundMessageState(OutboundMessageStatePtr const &oms)
		{
			uint32_t msgId = oms->getMsgId();

			m_outboundMessageStates[msgId] = oms;

			std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 10)));
			timer->async_wait(boost::bind(&PeerState::outboundTimerCallback, this, boost::asio::placeholders::error, msgId));

			m_outboundTimers[msgId] = timer;
		}

		void PeerState::delOutboundMessageState(const uint32_t msgId)
		{
			std::shared_ptr<boost::asio::deadline_timer> timer = m_outboundTimers[msgId];
			if(timer) {
				I2P_LOG(m_log, debug) << "canceling OMS timer";
				timer->cancel();
				m_outboundTimers.erase(msgId);
			}

			m_outboundMessageStates.erase(msgId);
		}

		void PeerState::outboundTimerCallback(const boost::system::error_code& e, const uint32_t msgId)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if(!e) {
				I2P_LOG(m_log, debug) << "removing OMS due to timeout";
				m_outboundMessageStates.erase(msgId);
				m_outboundTimers.erase(msgId);
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
