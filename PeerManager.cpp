#include "PeerManager.h"

#include "i2np/DeliveryStatus.h"

#include "RouterContext.h"

namespace i2pcpp {
	PeerManager::PeerManager(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_timer(m_ios, boost::posix_time::time_duration(0, 0, 1)),
		m_log(boost::log::keywords::channel = "PM") {}

	void PeerManager::begin()
	{
		m_timer.async_wait(boost::bind(&PeerManager::callback, this, boost::asio::placeholders::error));
	}

	void PeerManager::connected(const RouterHash rh)
	{
		// TODO Ding the peer's profile
	}

	void PeerManager::failure(const RouterHash rh)
	{
		// TODO Ding the peer's profile
	}

	void PeerManager::disconnected(const RouterHash rh)
	{
	}

	void PeerManager::callback(const boost::system::error_code &e)
	{
		try {
			//uint32_t maxPeers = std::stoi(m_ctx.getDatabase().getConfigValue("max_peers"));
			uint32_t numPeers = m_ctx.getOutMsgDisp().getTransport()->numPeers();

			I2P_LOG(m_log, debug) << "current number of peers: " << numPeers;
			
		} catch(std::exception &e) {
				I2P_LOG(m_log, error) << "exception in PeerManager: " << e.what();
		}

		m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 1));
		m_timer.async_wait(boost::bind(&PeerManager::callback, this, boost::asio::placeholders::error));
	}
}
