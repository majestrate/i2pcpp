#include "TunnelManager.h"

#include <iostream>

namespace i2pcpp {
	void TunnelManager::handleRequest(std::list<BuildRequestRecord> const &records)
	{
		RouterHash myIdentity = m_ctx.getMyRouterIdentity().getHash();
		std::array<unsigned char, 16> myTruncatedIdentity;
		std::copy(myIdentity.cbegin(), myIdentity.cbegin() + 16, myTruncatedIdentity.begin());

		for(auto& r: records) {
			if(myTruncatedIdentity == r.getHeader()) {
				std::cout << "TunnelManager: found BRR with our identity\n";
				BuildRequestRecord myRecord = r;
				myRecord.decrypt(m_ctx.getEncryptionKey());
			}
		}
	}
}
