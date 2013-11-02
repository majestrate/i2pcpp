#include "OutboundMessageDispatcher.h"

#include "RouterContext.h"

namespace i2pcpp {
	OutboundMessageDispatcher::OutboundMessageDispatcher(RouterContext &ctx) :
		m_ctx(ctx),
		m_log(boost::log::keywords::channel = "OMD") {}

	void OutboundMessageDispatcher::sendMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
	{
		if(!m_transport) throw std::logic_error("No transport registered");

		if(m_transport->isConnected(to))
			m_transport->send(to, msg->getMsgId(), msg->toBytes());
		else {
			I2P_LOG_SCOPED_RH(m_log, to);
			I2P_LOG(m_log, debug) << "not connected, queueing message";

			std::lock_guard<std::mutex> lock(m_mutex);

			m_pending.insert(MapType::value_type(to, msg));
			if(m_ctx.getDatabase().routerExists(to))
				m_transport->connect(m_ctx.getDatabase().getRouterInfo(to));
			else {
				I2P_LOG(m_log, debug) << "RouterInfo not in DB, searching DHT";

				auto k = DHT::Kademlia::makeKey(to);
				auto rh = m_ctx.getDHT()->find(k);
				if(rh != to) {
					if(rh == DHT::KademliaValue()) {
						I2P_LOG(m_log, error) << "could not find a good place to start search, aborting";
						m_pending.erase(to);

						return;
					}

					m_ctx.getSearchManager().createSearch(to, rh);
				} else
					I2P_LOG(m_log, error) << "RouterHash found in DHT but not DB";
			}
		}
	}

	void OutboundMessageDispatcher::queueMessage(RouterHash const &to, I2NP::MessagePtr const &msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_pending.insert(MapType::value_type(to, msg));
	}

	void OutboundMessageDispatcher::registerTransport(TransportPtr const &t)
	{
		m_transport = t;
	}

	TransportPtr OutboundMessageDispatcher::getTransport() const
	{
		return m_transport;
	}

	void OutboundMessageDispatcher::connected(RouterHash const rh)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto bucket = m_pending.equal_range(rh);
		for(auto itr = bucket.first; itr != bucket.second; ++itr) {
			I2P_LOG(m_log, debug) << "connected to peer, flushing queue";

			m_transport->send(itr->first, itr->second->getMsgId(), itr->second->toBytes());
		}
	}

	void OutboundMessageDispatcher::dhtSuccess(DHT::KademliaKey const k, DHT::KademliaValue const v)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if(m_pending.count(v)) {
			I2P_LOG_SCOPED_RH(m_log, v);
			I2P_LOG(m_log, debug) << "DHT lookup succeeded, connecting to peer";

			m_transport->connect(m_ctx.getDatabase().getRouterInfo(v));
		}
	}

	void OutboundMessageDispatcher::dhtFailure(DHT::KademliaKey const k)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if(m_pending.count(k)) {
			I2P_LOG_SCOPED_RH(m_log, k);
			I2P_LOG(m_log, debug) << "DHT lookup failed, tossing queued messages";

			m_pending.erase(k);
		}
	}
}
