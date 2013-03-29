#include "TunnelManager.h"

#include "../i2np/VariableTunnelBuild.h"

#include "../RouterContext.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(RouterContext &ctx) :
		m_ctx(ctx),
		m_log(boost::log::keywords::channel = "TM") {}

	void TunnelManager::createTunnel(bool inbound)
	{
		std::list<RouterHash> l = { std::string("DNJIjfMurt2-qg3kgBJJ4ShMq~WUJpMUW9Mh11LXo8A="), std::string("BMmZ54ID2g~ls3vaKqxNftEYZ9AtSXv8Mz4uKuy4PIk="), std::string("MwnGsCcpXZE2KOKUQbCHnqQqChyn2BcKvzyXX5s1XBw=") };
		TunnelState::Direction d = (inbound) ? TunnelState::INBOUND : TunnelState::OUTBOUND;
		TunnelStatePtr t = std::make_shared<TunnelState>(m_ctx, l, d);
		m_tunnels[t->getTunnelId()] = t;

		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRequest()));
		m_ctx.getOutMsgDisp().sendMessage(t->getTerminalHop(), vtb);
	}

	void TunnelManager::handleRequest(std::list<BuildRequestRecord> const &records)
	{
		RouterHash myIdentity = m_ctx.getIdentity().getHash();
		std::array<unsigned char, 16> myTruncatedIdentity;
		std::copy(myIdentity.cbegin(), myIdentity.cbegin() + 16, myTruncatedIdentity.begin());

		for(auto& r: records) {
			if(myTruncatedIdentity == r.getHeader()) {
				BOOST_LOG_SEV(m_log, debug) << "found BRR with our identity";
				BuildRequestRecord myRecord = r;
				myRecord.decrypt(m_ctx.getEncryptionKey());
			}
		}
	}
}
