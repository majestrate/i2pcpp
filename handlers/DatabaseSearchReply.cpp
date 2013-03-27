#include "DatabaseSearchReply.h"

#include "../i2np/DatabaseSearchReply.h"
#include "../i2np/DatabaseLookup.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DatabaseSearchReply::getType() const
		{
			return I2NP::Message::Type::DB_SEARCH_REPLY;
		}

		void DatabaseSearchReply::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::DatabaseSearchReply> dsr = std::dynamic_pointer_cast<I2NP::DatabaseSearchReply>(msg);

			std::cerr << "Received DatabaseSearchReply message from " << from << "\n";

			for(auto h: dsr->getHashes()) {
				if(!m_ctx.getDatabase().routerExists(h)) {
					I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(h, m_ctx.getIdentity().getHash(), 0));
					m_ctx.getOutMsgDisp().sendMessage(from, dbl);
				}
			}
		}
	}
}
