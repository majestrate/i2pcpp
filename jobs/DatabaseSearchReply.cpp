#include "DatabaseSearchReply.h"

#include "../util/Base64.h"
#include "../i2np/DatabaseSearchReply.h"
#include "../i2np/DatabaseLookup.h"
#include "../ssu/PacketBuilder.h"
#include "../OutboundMessageDispatcher.h"
#include "../Database.h"

#include <iostream>

namespace i2pcpp {
	namespace Jobs {
		void DatabaseSearchReply::run()
		{
			std::shared_ptr<I2NP::DatabaseSearchReply> dsr = std::dynamic_pointer_cast<I2NP::DatabaseSearchReply>(m_msg);

			std::cerr << "Received DatabaseSearchReply message from " << Base64::encode(ByteArray(m_from.cbegin(), m_from.cend())) << "\n";

			for(auto h: dsr->getHashes()) {
				if(!m_ctx.getDatabase().routerExists(h)) {
					I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(h, m_ctx.getMyRouterHash(), 0));
					m_ctx.getOutMsgDispatcher().sendMessage(m_from, dbl);
				}
			}
		}
	}
}
