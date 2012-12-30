#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../util/Base64.h"
#include "../i2np/DatabaseStore.h"
#include "../datatypes/RouterInfo.h"

#include <iostream>

namespace i2pcpp {
	namespace Jobs {
		void DatabaseStore::run()
		{
			try {
				std::shared_ptr<I2NP::DatabaseStore> dsm = std::dynamic_pointer_cast<I2NP::DatabaseStore>(m_msg);

				std::cerr << "Received DatabaseStore message from " << Base64::encode(ByteArray(m_from.cbegin(), m_from.cend())) << "\n";
				std::cerr << "DatabaseStore message reply token: " << dsm->getReplyToken() << "\n";

				Botan::Pipe gzPipe(new Botan::Zlib_Decompression);

				switch(dsm->getStoreType()) {
					case I2NP::DatabaseStore::Type::ROUTER_INFO:
						gzPipe.start_msg();
						gzPipe.write(dsm->getData());
						gzPipe.end_msg();
						break;

					case I2NP::DatabaseStore::Type::LEASE_SET:
						break;
				}

				unsigned int size = gzPipe.remaining();
				ByteArray inflatedData(size);
				gzPipe.read(inflatedData.data(), size);

				RouterInfo ri(inflatedData);

				std::cerr << "RouterInfo RouterAddress[0] host: " << ri.getAddress(0).getHost() << "\n";

			} catch(Botan::Decoding_Error &e) {
				std::cerr << "Jobs::DatabaseStore: Decoding error: " << e.what() << "\n";
			}
		}
	}
}
