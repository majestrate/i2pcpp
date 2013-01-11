#include "InboundMessageDispatcher.h"

#include <boost/bind.hpp>
#include <botan/pipe.h>
#include <botan/zlib.h>

#include "i2np/DatabaseStore.h"

#include "OutboundMessageDispatcher.h"

namespace i2pcpp {
	InboundMessageDispatcher::InboundMessageDispatcher(boost::asio::io_service &ios, RouterContext &ctx) :
		m_ios(ios),
		m_ctx(ctx),
		m_dbStoreHandler(ctx),
		m_dbSearchReplyHandler(ctx) {}

	void InboundMessageDispatcher::messageReceived(const RouterHash &from, const ByteArray &data)
	{
		I2NP::MessagePtr m = I2NP::Message::fromBytes(data);
		if(m) {
			switch(m->getType())
			{
				case I2NP::Message::Type::DB_STORE:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbStoreHandler, from, m));
					break;

				case I2NP::Message::Type::DB_SEARCH_REPLY:
					m_ios.post(boost::bind(&Handlers::Message::handleMessage, m_dbSearchReplyHandler, from, m));
					break;

				default:
					std::cerr << "Router: dropping unhandled message of type " << m->getType() << "\n";
					break;
			}
		}
	}

	void InboundMessageDispatcher::connectionEstablished(const RouterHash &rh)
	{
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

		unsigned int size = gzPipe.remaining();
		ByteArray gzInfoBytes(size);
		gzPipe.read(gzInfoBytes.data(), size);

		auto mydsm = std::make_shared<I2NP::DatabaseStore>(myInfo.getIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes);
		m_ctx.getOutMsgDispatcher().sendMessage(rh, mydsm);
	}
}
