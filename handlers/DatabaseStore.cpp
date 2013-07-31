#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../i2np/DatabaseStore.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace Handlers {
		DatabaseStore::DatabaseStore(RouterContext &ctx) :
			Message(ctx),
			m_log(boost::log::keywords::channel = "H[DSto]") {}

		I2NP::Message::Type DatabaseStore::getType() const
		{
			return I2NP::Message::Type::DB_STORE;
		}

		void DatabaseStore::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
		{
			try {
				std::shared_ptr<I2NP::DatabaseStore> dsm = std::dynamic_pointer_cast<I2NP::DatabaseStore>(msg);

				I2P_LOG_SCOPED_RH(m_log, from);
				I2P_LOG(m_log, debug) << "received DatabaseStore message";

				Botan::Pipe ungzPipe(new Botan::Zlib_Decompression);

				switch(dsm->getDataType()) {
					case I2NP::DatabaseStore::DataType::ROUTER_INFO:
						{
							ungzPipe.start_msg();
							ungzPipe.write(dsm->getData());
							ungzPipe.end_msg();

							unsigned int size = ungzPipe.remaining();
							ByteArray inflatedData(size);
							ungzPipe.read(inflatedData.data(), size);

							auto begin = inflatedData.cbegin();
							RouterInfo ri(begin, inflatedData.cend());

							if(ri.verify()) {
								m_ctx.getDatabase().setRouterInfo(ri);
								I2P_LOG(m_log, debug) << "added RouterInfo to DB";

								m_ctx.getSignals().invokeDatabaseStore(from, ri.getIdentity().getHash(), true);
							} else {
								I2P_LOG(m_log, error) << "RouterInfo verification failed";
							}
						}

						break;

					case I2NP::DatabaseStore::DataType::LEASE_SET:
						I2P_LOG(m_log, debug) << "this is a LeaseSet";

						// signal here
						break;
				}
			} catch(Botan::Decoding_Error &e) {
				I2P_LOG(m_log, error) << "problem decompressing data";
			}
		}
	}
}
