#include "TunnelManager.h"

#include "../RouterContext.h"

namespace i2pcpp {
	TunnelManager::TunnelManager(RouterContext &ctx) :
		m_ctx(ctx) {}

	void TunnelManager::handleRequest(std::list<BuildRequestRecord> const &records)
	{
		RouterHash myIdentity = m_ctx.getIdentity().getHash();
		std::array<unsigned char, 16> myTruncatedIdentity;
		std::copy(myIdentity.cbegin(), myIdentity.cbegin() + 16, myTruncatedIdentity.begin());

		for(auto& r: records) {
			if(myTruncatedIdentity == r.getHeader()) {
				BOOST_LOG_SEV(m_ctx.getLogger(), debug) << "found BRR with our identity";
				BuildRequestRecord myRecord = r;
				myRecord.decrypt(m_ctx.getEncryptionKey());
			}
		}
	}
}
