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
		uint32_t max_peers = std::stoi(m_ctx.getDatabase().getConfigValue("max_peers"));
		for(uint32_t i = 0; i < max_peers; i++)
			m_ctx.getOutMsgDisp().getTransport()->connect(m_ctx.getProfileManager().getPeer());

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
		uint32_t max_peers = std::stoi(m_ctx.getDatabase().getConfigValue("max_peers"));
		int32_t gap = max_peers - (m_inboundEstablished + m_outboundEstablished);

		for(int32_t i = 0; i < gap; i++)
			m_ctx.getOutMsgDisp().getTransport()->connect(m_ctx.getProfileManager().getPeer());

		timer->expires_at(timer->expires_at() + boost::posix_time::time_duration(0, 0, 10));
		timer->async_wait(boost::bind(&PeerManager::callback, this, boost::asio::placeholders::error, timer));
	}
}
