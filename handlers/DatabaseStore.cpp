#include "DatabaseStore.h"

#include <botan/pipe.h>
#include <botan/zlib.h>

#include "../Database.h"
#include "../util/Base64.h"
#include "../i2np/DatabaseStore.h"
#include "../OutboundMessageDispatcher.h"

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
				m_ctx.getDatabase().setRouterInfo(ri);
				std::cerr << "Added RouterInfo to DB\n";
			} else
				std::cerr << "RouterInfo verification failed\n";

			if(m_autoReply) {
				Mapping am;
				am.setValue("caps", "BC");
				am.setValue("host", "127.0.0.1");
				am.setValue("key", m_ctx.getMyRouterIdentity().getHashEncoded());
				am.setValue("port", "27333");
				RouterAddress a(5, Date(0), "SSU", am);

				Mapping rm;
				rm.setValue("coreVersion", "0.9.4");
				rm.setValue("netId", "2");
				rm.setValue("router.version", "0.9.4");
				rm.setValue("stat_uptime", "90m");
				RouterInfo myInfo(m_ctx.getMyRouterIdentity(), Date(), rm);
				myInfo.addAddress(a);
				myInfo.sign(m_ctx.getSigningKey());

				Botan::Pipe gzPipe(new Botan::Zlib_Compression);
				gzPipe.start_msg();
				gzPipe.write(myInfo.getBytes());
				gzPipe.end_msg();

				size = gzPipe.remaining();
				ByteArray gzInfoBytes(size);
				gzPipe.read(gzInfoBytes.data(), size);

				I2NP::MessagePtr mydsm(new I2NP::DatabaseStore(myInfo.getIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes));
				m_ctx.getOutMsgDispatcher().sendMessage(from, mydsm);

				m_autoReply = false;
			}
		}
	}
}
