#include "TunnelManager.h"

#include "../i2np/VariableTunnelBuild.h"

#include "../RouterContext.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(RouterContext &ctx) :
		m_ctx(ctx),
		m_log(boost::log::keywords::channel = "TM") {}

	void TunnelManager::receiveRecords(std::list<BuildRecord> records)
	{
		RouterHash myHash = m_ctx.getIdentity().getHash();
		std::array<unsigned char, 16> myTruncatedHash;
		std::copy(myHash.cbegin(), myHash.cbegin() + 16, myTruncatedHash.begin());

		for(auto& r: records) {
			if(myTruncatedHash == r.getHeader()) {
				I2P_LOG(m_log, debug) << "found BRR with our identity";
				r.decrypt(m_ctx.getEncryptionKey());

				std::lock_guard<std::mutex> lock(m_tunnelsMutex);
				BuildRequestRecord req = r;
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
					BuildResponseRecord resp = r;
					handleResponse(resp);
				} else {
					I2P_LOG(m_log, debug) << "did not find Tunnel with matching Tunnel ID (participation request)";

					std::lock_guard<std::mutex> lock(m_participatingMutex);
					if(m_participating.count(hop.getTunnelId()) > 0) {
						I2P_LOG(m_log, debug) << "rejecting tunnel participation request: tunnel ID in use";
						// reject
						return;
					}

					m_participating[hop.getTunnelId()] = std::make_shared<TunnelHop>(hop);
					BuildResponseRecord resp(BuildResponseRecord::SUCCESS);
					resp.setHeader(req.getHeader());
					resp.compile();

					r = resp; // Replace the request record with our response

					for(auto& x: records)
						x.encrypt(hop.getReplyIV(), hop.getReplyKey());

					I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(records));
					m_ctx.getOutMsgDisp().sendMessage(hop.getNextHash(), vtb);

					break;
				}
			}
		}
	}

	void TunnelManager::handleResponse(BuildResponseRecord &response)
	{
	}
}
