#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../util/Base64.h"
#include "../i2np/DatabaseStore.h"
#include "../datatypes/RouterInfo.h"
#include "../ssu/PacketBuilder.h"
#include "../Database.h"

#include <iostream>

namespace i2pcpp {
	namespace Jobs {
		void DatabaseStore::run()
		{
			try {
				std::shared_ptr<I2NP::DatabaseStore> dsm = std::dynamic_pointer_cast<I2NP::DatabaseStore>(m_msg);

				std::cerr << "Received DatabaseStore message from " << Base64::encode(ByteArray(m_from.cbegin(), m_from.cend())) << "\n";
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

				std::cerr << "RouterInfo RouterAddress[0] host: " << ri.getAddress(0).getOptions().getValue("host") << "\n";

				if(ri.verifySignature(m_ctx)) {
					m_ctx.getDatabase().setRouterInfo(ri);
					std::cerr << "Added RouterInfo to DB\n";
				} else
					std::cerr << "RouterInfo verification failed\n";

				RouterInfo myInfo(m_ctx.getRouterIdentity(), Date(), Mapping(), ByteArray(40));
				myInfo.sign(m_ctx);

				Botan::Pipe gzPipe(new Botan::Zlib_Compression);
				gzPipe.start_msg();
				gzPipe.write(myInfo.getBytes());
				gzPipe.end_msg();

				size = gzPipe.remaining();
				ByteArray gzInfoBytes(size);
				gzPipe.read(gzInfoBytes.data(), size);

				I2NP::DatabaseStore mydsm(m_ctx.getRouterIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes);
				/*SSU::PacketBuilder pb;
				SSU::PacketPtr p = pb.buildData(ps, mydsm);*/

			} catch(Botan::Decoding_Error &e) {
				std::cerr << "Jobs::DatabaseStore: Decoding error: " << e.what() << "\n";
			}
		}
	}
}
