#include "OutboundMessageDispatcher.h"

#include "RouterContext.h"

namespace i2pcpp {
	OutboundMessageDispatcher::OutboundMessageDispatcher(RouterContext &ctx) :
		m_ctx(ctx),
		m_log(boost::log::keywords::channel = "OMD") {}

	void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
	{
		if(!m_transport) return; // TODO Exception

		if(m_transport->isConnected(to))
			m_transport->send(to, msg->toBytes());
		else {
			std::lock_guard<std::mutex> lock(m_mutex);

			m_pending.insert(MapType::value_type(to, msg));
			if(m_ctx.getDatabase().routerExists(to))
				m_transport->connect(m_ctx.getDatabase().getRouterInfo(to));
		}
	}

	void OutboundMessageDispatcher::registerTransport(TransportPtr const &t)
	{
		m_transport = t;
	}

	TransportPtr OutboundMessageDispatcher::getTransport() const
	{
		return m_transport;
	}

	void OutboundMessageDispatcher::connected(RouterHash const &rh)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		I2P_LOG(m_log, debug) << "count: " << m_pending.count(rh);
		auto bucket = m_pending.equal_range(rh);
		for(auto itr = bucket.first; itr != bucket.second; ++itr) {
			m_transport->send(itr->first, itr->second->toBytes());
			I2P_LOG(m_log, debug) << "sending!";
		}
	}

	void OutboundMessageDispatcher::infoSaved(RouterHash const &rh)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if(m_pending.count(rh)) {
			m_transport->connect(m_ctx.getDatabase().getRouterInfo(rh));
		}

		I2P_LOG(m_log, debug) << "info saved!";
	}
}
