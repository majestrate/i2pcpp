#include "TunnelManager.h"

#include "../i2np/VariableTunnelBuild.h"

#include "../RouterContext.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(RouterContext &ctx) :
		m_ctx(ctx),
		m_log(boost::log::keywords::channel = "TM") {}

	void TunnelManager::createTunnel(bool inbound)
	{
	  // hard coded
		std::list<RouterHash> l = { std::string("DNJIjfMurt2-qg3kgBJJ4ShMq~WUJpMUW9Mh11LXo8A="), std::string("BMmZ54ID2g~ls3vaKqxNftEYZ9AtSXv8Mz4uKuy4PIk="), std::string("MwnGsCcpXZE2KOKUQbCHnqQqChyn2BcKvzyXX5s1XBw=") };
		TunnelState::Direction d = (inbound) ? TunnelState::INBOUND : TunnelState::OUTBOUND;
		TunnelStatePtr t = std::make_shared<TunnelState>(m_ctx, l, d);

		std::lock_guard<std::mutex> lock(m_tunnelsMutex);
		m_tunnels[t->getTunnelId()] = t;

		I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(t->getRequest()));
		m_ctx.getOutMsgDisp().sendMessage(t->getTerminalHop(), vtb);
	}

  void TunnelManager::mainloop()
  {


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

				std::lock_guard<std::mutex> lock(m_tunnelsMutex);
				auto itr = m_tunnels.find(myRecord.getTunnelId());
				if(itr != m_tunnels.end()) {
					TunnelStatePtr ts = itr->second;

					BOOST_LOG_SEV(m_log, debug) << "found TunnelState with matching tunnel ID";

					std::list<BuildResponseRecord> responses(records.cbegin(), records.cend());
					ts->parseResponseRecords(responses);

					switch(ts->getState()) {
						case TunnelState::OPERATIONAL:
							BOOST_LOG_SEV(m_log, debug) << "tunnel built successfully";
							break;

						case TunnelState::FAILURE:
							BOOST_LOG_SEV(m_log, debug) << "tunnel build failed";
							break;

						default:
							break;
					}
				} else {
					BOOST_LOG_SEV(m_log, debug) << "TunnelState with matching tunnel ID not found";
				}
			}
		}
	}
}
