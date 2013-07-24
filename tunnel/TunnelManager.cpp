#include "TunnelManager.h"

#include "../i2np/VariableTunnelBuild.h"
#include "../i2np/VariableTunnelBuildReply.h"

#include "../RouterContext.h"

#include "InboundTunnel.h"
#include "OutboundTunnel.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_generator(m_ctx.getDatabase()),
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

				BuildRequestRecord req = *r;
				req.decrypt(m_ctx.getEncryptionKey());

				std::lock_guard<std::mutex> lock(m_tunnelsMutex);
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

					t->handleResponses(records);
					if(t->getState() == Tunnel::OPERATIONAL) {
						I2P_LOG(m_log, debug) << "tunnel is operational";
					} else {
						I2P_LOG(m_log, debug) << "failed to build tunnel";

						m_tunnels.erase(itr);
					}
				} else {
					I2P_LOG(m_log, debug) << "did not find Tunnel with matching Tunnel ID (participation request)";

					std::lock_guard<std::mutex> lock(m_participatingMutex);
					if(m_participating.count(hop.getTunnelId()) > 0) {
						I2P_LOG(m_log, debug) << "rejecting tunnel participation request: tunnel ID in use";
						// reject
						return;
					}

					auto resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::SUCCESS);
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

	void TunnelManager::receiveGatewayData(uint32_t const tunnelId, ByteArray const data)
	{
		I2P_LOG(m_log, debug) << "received " << data.size() << " bytes for tunnel " << tunnelId;

		I2NP::MessagePtr msg = I2NP::Message::fromBytes(data, true);
		if(msg && msg->getType() == I2NP::Message::VARIABLE_TUNNEL_BUILD_REPLY) {
			std::shared_ptr<I2NP::VariableTunnelBuildReply> vtbr = std::dynamic_pointer_cast<I2NP::VariableTunnelBuildReply>(msg);
			I2P_LOG(m_log, debug) << "VTBR with " << vtbr->getRecords().size() << " records";
		}
	}

	void TunnelManager::callback(const boost::system::error_code &e)
	{
		for (auto counter =0; counter < 5; counter++) createTunnel();
		m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 1));
		m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));
	}

	void TunnelManager::createTunnel()
	{	
		
		I2P_LOG(m_log, debug) << "creating tunnel";
		auto hops = m_generator.makeTunnelHops(3);
		auto t = std::make_shared<InboundTunnel>(hops);
		m_tunnels[t->getTunnelId()] = t;
		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRecords()));
		m_ctx.getOutMsgDisp().sendMessage(t->getDownstream(), vtb);
	}
}
