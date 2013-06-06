#include "PeerManager.h"

#include "RouterContext.h"

namespace i2pcpp {
	PeerManager::PeerManager(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_inboundEstablished(0),
		m_outboundEstablished(0),
		m_log(boost::log::keywords::channel = "PM") {}

	void PeerManager::begin()
	{
		std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 10)));
		timer->async_wait(boost::bind(&PeerManager::callback, this, boost::asio::placeholders::error, timer));
	}

	void PeerManager::establishmentSuccess(const RouterHash rh, bool inbound)
	{
		// TODO Ding the peer's profile

		if(inbound)
			m_inboundEstablished++;
		else
			m_outboundEstablished++;
	}

	void PeerManager::establishmentFailure(const RouterHash rh)
	{
		// TODO Ding the peer's profile
	}

	void PeerManager::callback(const boost::system::error_code &e, std::shared_ptr<boost::asio::deadline_timer> timer)
	{
		uint32_t maxPeers = std::stoi(m_ctx.getDatabase().getConfigValue("max_peers"));
		uint32_t numPeers = m_ctx.getOutMsgDisp().getTransport()->numPeers();

		I2P_LOG(m_log, debug) << "current number of peers: " << numPeers << " (IB: " << m_inboundEstablished << ", OB: " << m_outboundEstablished << ")";

		uint32_t gap = maxPeers - numPeers;
		for(int32_t i = 0; i < gap; i++)
			m_ctx.getOutMsgDisp().getTransport()->connect(m_ctx.getProfileManager().getPeer());

		timer->expires_at(timer->expires_at() + boost::posix_time::time_duration(0, 0, 10));
		timer->async_wait(boost::bind(&PeerManager::callback, this, boost::asio::placeholders::error, timer));
	}
}
