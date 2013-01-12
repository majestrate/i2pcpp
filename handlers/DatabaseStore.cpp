#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../Database.h"
#include "../util/Base64.h"
#include "../i2np/DatabaseStore.h"
#include "../OutboundMessageDispatcher.h"
#include "../Signals.h"

namespace i2pcpp {
	namespace Handlers {
		I2NP::Message::Type DatabaseStore::getType() const
		{
			return I2NP::Message::Type::DB_STORE;
		}

		void DatabaseStore::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::DatabaseStore> dsm = std::dynamic_pointer_cast<I2NP::DatabaseStore>(msg);

			std::cerr << "Received DatabaseStore message from " << Base64::encode(ByteArray(from.cbegin(), from.cend())) << "\n";
			std::cerr << "DatabaseStore message reply token: " << dsm->getReplyToken() << "\n";

			Botan::Pipe ungzPipe(new Botan::Zlib_Decompression);

			switch(dsm->getDataType()) {
				case I2NP::DatabaseStore::DataType::ROUTER_INFO:
					ungzPipe.start_msg();
					ungzPipe.write(dsm->getData());
					ungzPipe.end_msg();
					break;

				case I2NP::DatabaseStore::DataType::LEASE_SET:
					break;
			}

			unsigned int size = ungzPipe.remaining();
			ByteArray inflatedData(size);
			ungzPipe.read(inflatedData.data(), size);

			RouterInfo ri(inflatedData);

			//std::cerr << "RouterInfo RouterAddress[0] host: " << ri.getAddress(0).getOptions().getValue("host") << "\n";

			if(ri.verifySignature(m_ctx.getDSAParameters())) {
				//m_ctx.getDatabase().setRouterInfo(ri);
				std::cerr << "Added RouterInfo to DB\n";
				m_ctx.getSignals().invokeRouterInfoSaved(ri.getIdentity().getHash());
			} else
				std::cerr << "RouterInfo verification failed\n";
		}
	}
}
