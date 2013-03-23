#ifndef SSUPEERSTATE_H
#define SSUPEERSTATE_H

#include <boost/asio.hpp>

#include "../../datatypes/RouterIdentity.h"
#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"

#include "InboundMessageState.h"

namespace i2pcpp {
	namespace SSU {
		class PeerState {
			public:
				PeerState(boost::asio::io_service &ios, Endpoint const &ep, RouterIdentity const &ri);

				InboundMessageStatePtr getInboundMessageState(const uint32_t msgId) const;
				void addInboundMessageState(InboundMessageStatePtr const &ims);
				void delInboundMessageState(const uint32_t msgId);
				void delInboundMessageState(std::map<uint32_t, InboundMessageStatePtr>::const_iterator itr);

				void inboundTimerCallback(const boost::system::error_code& e, const uint32_t msgId);

				SessionKey getCurrentSessionKey() const;
				SessionKey getCurrentMacKey() const;
				SessionKey getNextSessionKey() const;
				SessionKey getNextMacKey() const;

				void setCurrentSessionKey(SessionKey const &sk);
				void setCurrentMacKey(SessionKey const &mk);
				void setNextSessionKey(SessionKey const &sk);
				void setNextMacKey(SessionKey const &mk);

				const RouterIdentity& getIdentity() const;
				const Endpoint& getEndpoint() const;

				std::mutex& getMutex() const;

				std::map<uint32_t, InboundMessageStatePtr>::iterator begin();
				std::map<uint32_t, InboundMessageStatePtr>::iterator end();

			private:
				boost::asio::io_service &m_ios;

				Endpoint m_endpoint;
				RouterIdentity m_identity;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				SessionKey m_nextSessionKey;
				SessionKey m_nextMacKey;

				std::map<uint32_t, InboundMessageStatePtr> m_inboundMessageStates;
				std::map<uint32_t, std::shared_ptr<boost::asio::deadline_timer>> m_inboundTimers;

				mutable std::mutex m_mutex;
		};

		typedef std::shared_ptr<PeerState> PeerStatePtr;
	}
}

template<>
struct std::hash<i2pcpp::SSU::PeerStatePtr> {
	public:
		size_t operator()(const i2pcpp::SSU::PeerStatePtr &ps) const
		{
			std::hash<i2pcpp::RouterHash> f;
			return f(ps->getIdentity().getHash());
		}
};

#endif