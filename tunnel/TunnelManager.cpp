#include "TunnelManager.h"

#include <botan/auto_rng.h>

#include "../i2np/VariableTunnelBuild.h"
#include "../i2np/VariableTunnelBuildReply.h"
#include "../i2np/TunnelData.h"
#include "../i2np/TunnelGateway.h"

#include "../RouterContext.h"

#include "InboundTunnel.h"
#include "OutboundTunnel.h"
#include "Message.h"
#include "Fragment.h"
#include "FragmentHandler.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_fragmentHandler(ctx),
		m_timer(m_ios, boost::posix_time::time_duration(0, 0, 1)),
		m_log(boost::log::keywords::channel = "TM") {}

	void TunnelManager::begin()
	{
		m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));
	}

	void TunnelManager::receiveRecords(uint32_t const msgId, std::list<BuildRecordPtr> records)
	{
		{
			std::lock_guard<std::mutex> lock(m_pendingMutex);
			auto itr = m_pending.find(msgId);
			if(itr != m_pending.end()) {
				TunnelPtr t = itr->second;

				if(t->getState() != Tunnel::State::REQUESTED) {
					I2P_LOG(m_log, debug) << "found Tunnel with matching tunnel ID, but was not requested";
					// reject
					return;
				}

				t->handleResponses(records);
				if(t->getState() == Tunnel::State::OPERATIONAL) {
					I2P_LOG(m_log, debug) << "tunnel is operational";

					std::lock_guard<std::mutex> lock(m_tunnelsMutex);
					m_tunnels[t->getTunnelId()] = t;
				} else {
					I2P_LOG(m_log, debug) << "failed to build tunnel";
				}

				m_pending.erase(itr);
				return;
			}
		}

		RouterHash myHash = m_ctx.getIdentity()->getHash();
		StaticByteArray<16> myTruncatedHash;
		std::copy(myHash.cbegin(), myHash.cbegin() + 16, myTruncatedHash.begin());

		auto itr = std::find_if(records.begin(), records.end(), [myTruncatedHash](BuildRecordPtr const &r) { return (myTruncatedHash == r->getHeader()); });
		if(itr != records.end()) {
			I2P_LOG(m_log, debug) << "found BRR with our identity";

			BuildRequestRecord req = **itr;
			req.decrypt(m_ctx.getEncryptionKey());
			TunnelHop hop = req.parse();

			std::lock_guard<std::mutex> lock(m_participatingMutex);
			if(m_participating.count(hop.getTunnelId()) > 0) {
				I2P_LOG(m_log, debug) << "rejecting tunnel participation request: tunnel ID in use";
				// reject
				return;
			}

			m_participating[hop.getTunnelId()] = std::make_shared<TunnelHop>(hop);

			BuildResponseRecordPtr resp;
			resp = std::make_shared<BuildResponseRecord>(BuildResponseRecord::Reply::SUCCESS);
			resp->compile();
			*itr = std::move(resp); // Replace the request record with our response

			for(auto& x: records)
				x->encrypt(hop.getReplyIV(), hop.getReplyKey());

			if(hop.getType() == TunnelHop::Type::ENDPOINT) {
				I2P_LOG(m_log, debug) << "forwarding BRRs to IBGW: " << hop.getNextHash() << ", tunnel ID: " << hop.getNextTunnelId() << ", nextMsgId: " << hop.getNextMsgId();

				I2NP::MessagePtr vtbr(new I2NP::VariableTunnelBuildReply(hop.getNextMsgId(), records));
				I2NP::MessagePtr tg(new I2NP::TunnelGateway(hop.getNextTunnelId(), vtbr->toBytes()));
				m_ctx.getOutMsgDisp().sendMessage(hop.getNextHash(), tg);
			} else {
				I2P_LOG(m_log, debug) << "forwarding BRRs to next hop: " << hop.getNextHash() << ", tunnel ID: " << hop.getNextTunnelId() << ", nextMsgId: " << hop.getNextMsgId();

				I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(hop.getNextMsgId(), records));
				m_ctx.getOutMsgDisp().sendMessage(hop.getNextHash(), vtb);
			}
		}
	}

	void TunnelManager::receiveGatewayData(RouterHash const from, uint32_t const tunnelId, ByteArray const data)
	{
		I2P_LOG_SCOPED_TAG(m_log, "TunnelId", tunnelId);
		I2P_LOG(m_log, debug) << "received " << data.size() << " bytes of gateway data";

		{
			std::lock_guard<std::mutex> lock(m_participatingMutex);
			auto itr = m_participating.find(tunnelId);
			if(itr != m_participating.end()) {
				TunnelHopPtr hop = itr->second;

				if(hop->getType() != TunnelHop::Type::GATEWAY) {
					I2P_LOG(m_log, debug) << "data is for a tunnel which is not a gateway, dropping";
					return;
				}

				I2P_LOG(m_log, debug) << "data is for a known tunnel, encrypting and forwarding";
				SessionKey k1 = hop->getTunnelIVKey();
				Botan::SymmetricKey ivKey(k1.data(), k1.size());
				SessionKey k2 = hop->getTunnelLayerKey();
				Botan::SymmetricKey layerKey(k2.data(), k2.size());

				auto fragments = Fragment::fragmentMessage(data);
				I2P_LOG(m_log, debug) << "we have " << fragments.size() << " fragments";

				for(auto& f: fragments) {
					I2P_LOG(m_log, debug) << "fragment: " << f->compile();

					std::list<FragmentPtr> x;
					x.push_back(std::move(f)); // This may or may not be unsafe

					Message msg(x);
					msg.compile();
					msg.encrypt(ivKey, layerKey);
					I2NP::MessagePtr td(new I2NP::TunnelData(hop->getNextTunnelId(), msg.getEncryptedData()));
					m_ctx.getOutMsgDisp().sendMessage(hop->getNextHash(), td);
				}

				return;
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_tunnelsMutex);
			auto itr = m_tunnels.find(tunnelId);
			if(itr != m_tunnels.end()) {
				TunnelPtr t = itr->second;

				if(t->getDirection() == Tunnel::Direction::INBOUND && t->getState() == Tunnel::State::OPERATIONAL) {
					I2P_LOG(m_log, debug) << "data is for one of our own tunnels, recirculating";
					std::shared_ptr<InboundTunnel> ibt = std::dynamic_pointer_cast<InboundTunnel>(t);

					m_ios.post(boost::bind(&InboundMessageDispatcher::messageReceived, boost::ref(m_ctx.getInMsgDisp()), from, 0, data));
				}
			}
		}
	}

	void TunnelManager::receiveData(RouterHash const from, uint32_t const tunnelId, StaticByteArray<1024> const data)
	{
		std::lock_guard<std::mutex> lock(m_participatingMutex);

		I2P_LOG_SCOPED_TAG(m_log, "TunnelId", tunnelId);
		I2P_LOG(m_log, debug) << "received " << data.size() << " bytes of tunnel data";

		auto itr = m_participating.find(tunnelId);
		if(itr != m_participating.end()) {
			I2P_LOG(m_log, debug) << "data is for a known tunnel";

			TunnelHopPtr hop = itr->second;

			SessionKey k1 = hop->getTunnelIVKey();
			Botan::SymmetricKey ivKey(k1.data(), k1.size());

			SessionKey k2 = hop->getTunnelLayerKey();
			Botan::SymmetricKey layerKey(k2.data(), k2.size());

			Message msg(data);
			msg.encrypt(ivKey, layerKey);

			switch(hop->getType()) {
				case TunnelHop::Type::PARTICIPANT:
					{
						I2P_LOG(m_log, debug) << "we are a participant, forwarding";

						I2NP::MessagePtr td(new I2NP::TunnelData(hop->getNextTunnelId(), msg.getEncryptedData()));
						m_ctx.getOutMsgDisp().sendMessage(hop->getNextHash(), td);
					}

					break;

				case TunnelHop::Type::ENDPOINT:
					{
						I2P_LOG(m_log, debug) << "we are an endpoint, sending to fragment handler";

						m_fragmentHandler.receiveFragments(msg.parse());
					}

					break;

				default:
					break;
			}
		} else
			I2P_LOG(m_log, debug) << "data is for an unknown tunnel, dropping";
	}

	void TunnelManager::callback(const boost::system::error_code &e)
	{
		createTunnel();

		m_timer.expires_at(m_timer.expires_at() + boost::posix_time::time_duration(0, 0, 5));
		m_timer.async_wait(boost::bind(&TunnelManager::callback, this, boost::asio::placeholders::error));
	}

	void TunnelManager::createTunnel()
	{
		I2P_LOG(m_log, debug) << "creating tunnel";
		std::vector<RouterIdentity> hops = { m_ctx.getProfileManager().getPeer().getIdentity() };

		auto z = std::make_shared<InboundTunnel>(m_ctx.getIdentity()->getHash());
		auto t = std::make_shared<OutboundTunnel>(hops, m_ctx.getIdentity()->getHash(), z->getTunnelId());
		//auto t = std::make_shared<InboundTunnel>(m_ctx.getIdentity().getHash(), hops);
		m_tunnels[t->getTunnelId()] = z;
		m_pending[t->getNextMsgId()] = t;

		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRecords()));
		m_ctx.getOutMsgDisp().sendMessage(t->getDownstream(), vtb);
	}
}
