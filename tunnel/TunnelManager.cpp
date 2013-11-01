#include "TunnelManager.h"

#include <botan/auto_rng.h>

#include "../i2np/VariableTunnelBuild.h"
#include "../i2np/VariableTunnelBuildReply.h"
#include "../i2np/TunnelData.h"

#include "../RouterContext.h"

#include "InboundTunnel.h"
#include "OutboundTunnel.h"
#include "TunnelMessage.h"

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

				BuildRequestRecord req = *r;
				req.decrypt(m_ctx.getEncryptionKey());
				req.parse();
				TunnelHop& hop = req.getHop();

				std::lock_guard<std::mutex> lock(m_tunnelsMutex);
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

					BuildResponseRecordPtr resp;

					if(hop.getType() != TunnelHop::PARTICIPANT) {
						I2P_LOG(m_log, debug) << "rejecting tunnel participation request: gateways and endpoints not implemented yet";

						resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::PROBABALISTIC_REJECT);
					} else {
						m_participating[hop.getTunnelId()] = std::make_shared<TunnelHop>(hop);

						resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::SUCCESS);
					}

					resp->compile();

					r = resp; // Replace the request record with our response

					for(auto& x: records)
						x->encrypt(hop.getReplyIV(), hop.getReplyKey());

					I2P_LOG(m_log, debug) << "forwarding BRRs to next hop: " << hop.getNextHash() << ", tunnel ID: " << hop.getTunnelId() << ", nextMsgId: " << hop.getNextMsgId();

					I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(hop.getNextMsgId(), records));
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

	void TunnelManager::receiveData(uint32_t const tunnelId, std::array<unsigned char, 1024> const data)
	{
		I2P_LOG(m_log, debug) << "received data for tunnel " << tunnelId;

		auto itr = m_participating.find(tunnelId);
		if(itr != m_participating.end()) {
			I2P_LOG(m_log, debug) << "data is for a known tunnel, encrypting and forwarding";

			TunnelHopPtr hop = m_participating[tunnelId];

			SessionKey k1 = hop->getTunnelIVKey();
			Botan::SymmetricKey ivKey(k1.data(), k1.size());

			SessionKey k2 = hop->getTunnelLayerKey();
			Botan::SymmetricKey layerKey(k2.data(), k2.size());

			TunnelMessage msg(data);
			msg.encrypt(ivKey, layerKey);

			I2NP::MessagePtr td(new I2NP::TunnelData(hop->getNextTunnelId(), msg.compile()));
			m_ctx.getOutMsgDisp().sendMessage(hop->getNextHash(), td);
		} else
			I2P_LOG(m_log, debug) << "data is for an unknown tunnel, dropping";
	}

	void TunnelManager::callback(const boost::system::error_code &e)
	{
		createTunnel();

		//m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 5));
		//m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));
	}

	void TunnelManager::createTunnel()
	{
		I2P_LOG(m_log, debug) << "creating tunnel";
		std::vector<RouterIdentity> hops = { m_ctx.getProfileManager().getPeer().getIdentity() };

		uint32_t tunnelId;
		Botan::AutoSeeded_RNG rng;
		rng.randomize((unsigned char *)&tunnelId, sizeof(tunnelId));

		auto t = std::make_shared<OutboundTunnel>(hops, m_ctx.getIdentity().getHash(), tunnelId);
		m_tunnels[t->getTunnelId()] = t;
		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRecords()));
		m_ctx.getOutMsgDisp().sendMessage(t->getDownstream(), vtb);
	}
}
