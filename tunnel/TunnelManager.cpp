#include "TunnelManager.h"

#include "../i2np/VariableTunnelBuild.h"

#include "../RouterContext.h"

#include "InboundTunnel.h"
#include "OutboundTunnel.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_timer(m_ios, boost::posix_time::time_duration(0, 0, 1)),
		m_log(boost::log::keywords::channel = "TM") {}

	void TunnelManager::begin()
	{
		m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));
	}

	void TunnelManager::receiveRecords(std::list<BuildRecordPtr> records)
	{
		RouterHash myHash = m_ctx.getIdentity().getHash();
		std::array<unsigned char, 16> myTruncatedHash;
		std::copy(myHash.cbegin(), myHash.cbegin() + 16, myTruncatedHash.begin());

		for(auto& r: records) {
			if(myTruncatedHash == r->getHeader()) {
				I2P_LOG(m_log, debug) << "found BRR with our identity";
				r->decrypt(m_ctx.getEncryptionKey());

				std::lock_guard<std::mutex> lock(m_tunnelsMutex);
				BuildRequestRecord req = *r;
				req.parse();
				TunnelHop& hop = req.getHop();
				auto itr = m_tunnels.find(hop.getTunnelId());
				if(itr != m_tunnels.end()) {
					TunnelPtr t = itr->second;

					if(t->getState() != Tunnel::REQUESTED) {
						I2P_LOG(m_log, debug) << "found Tunnel with matching tunnel ID, but was not requested";
						// reject
						return;
					}

					I2P_LOG(m_log, debug) << "found requested Tunnel with matching tunnel ID";
					BuildResponseRecord resp = *r;
					// handle response
				} else {
					I2P_LOG(m_log, debug) << "did not find Tunnel with matching Tunnel ID (participation request)";

					std::lock_guard<std::mutex> lock(m_participatingMutex);
					if(m_participating.count(hop.getTunnelId()) > 0) {
						I2P_LOG(m_log, debug) << "rejecting tunnel participation request: tunnel ID in use";
						// reject
						return;
					}

					auto resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::SUCCESS);
					resp->setHeader(req.getHeader());
					resp->compile();

					r = resp; // Replace the request record with our response

					for(auto& x: records)
						x->encrypt(hop.getReplyIV(), hop.getReplyKey());

					I2P_LOG(m_log, debug) << "forwarding BRRs to next hop: " << hop.getNextHash();

					I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(records));
					m_ctx.getOutMsgDisp().sendMessage(hop.getNextHash(), vtb);

					break;
				}
			}
		}
	}

	void TunnelManager::callback(const boost::system::error_code &e)
	{
		createTunnel();

		/*m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 10));
		m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));*/
	}

	void TunnelManager::createTunnel()
	{
		I2P_LOG(m_log, debug) << "creating tunnel";
		std::vector<RouterIdentity> hops = { m_ctx.getIdentity(), m_ctx.getDatabase().getRouterInfo("SXnw0C~04DNl~FWY0u1ApL7n-zSc2RIlrnYT6EqoAyU=").getIdentity() };
		InboundTunnel t(hops);
		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t.getRecords()));
		m_ctx.getOutMsgDisp().sendMessage(t.getDownstream(), vtb);
	}
}
