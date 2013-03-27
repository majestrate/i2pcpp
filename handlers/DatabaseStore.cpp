#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../i2np/DatabaseStore.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		DatabaseStore::DatabaseStore(RouterContext &ctx) :
			Message(ctx) {}

		I2NP::Message::Type DatabaseStore::getType() const
		{
			return I2NP::Message::Type::DB_STORE;
		}

		void DatabaseStore::handleMessage(RouterHash const &from, I2NP::MessagePtr const &msg)
		{
			std::shared_ptr<I2NP::DatabaseStore> dsm = std::dynamic_pointer_cast<I2NP::DatabaseStore>(msg);

			I2P_LOG_RH(m_ctx.getLogger(), from);
			BOOST_LOG_SEV(m_ctx.getLogger(), debug) << "received DatabaseStore message";

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

			auto begin = inflatedData.cbegin();
			RouterInfo ri(begin, inflatedData.cend());

			if(ri.verifySignature(m_ctx.getDSAParameters())) {
				m_ctx.getDatabase().setRouterInfo(ri);
				BOOST_LOG_SEV(m_ctx.getLogger(), debug) << "added RouterInfo to DB";
				m_ctx.getSignals().invokeRouterInfoSaved(ri.getIdentity().getHash());
			} else {
				BOOST_LOG_SEV(m_ctx.getLogger(), error) << "RouterInfo verification failed";
			}
		}
	}
}
