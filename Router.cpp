#include "Router.h"

#include "i2np/Message.h"

/* TEMPORARY */
#include <botan/pipe.h>
#include <botan/zlib.h>
#include "util/Base64.h"
#include "i2np/DatabaseLookup.h"
#include "i2np/DatabaseStore.h"
#include "ssu/UDPTransport.h"

namespace i2pcpp {
	Router::~Router()
	{
		m_serviceThread.join();
	}

	void Router::start()
	{
		m_serviceThread = std::thread([&](){m_ios.run();});

		m_transport = TransportPtr(new SSU::UDPTransport(m_ctx));
		m_transport->registerReceivedHandler(boost::bind(&Router::messageReceived, this, _1, _2));
		m_transport->registerEstablishedHandler(boost::bind(&Router::connectionEstablished, this, _1));
		m_outMsgDispatcher.registerTransport(m_transport);

		std::shared_ptr<SSU::UDPTransport> u = std::dynamic_pointer_cast<SSU::UDPTransport>(m_transport);
		u->start(Endpoint("127.0.0.1", 27333)); // TODO Config setting
		u->connect(Base64::decode("zhPja0k1cboGnHbhqO50hNPTVHIRE8b4GMwi7Htey~E="));
	}

	void Router::stop()
	{
		m_ios.stop();
	}

	void Router::messageReceived(const RouterHash &from, const ByteArray &data)
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

	void Router::connectionEstablished(const RouterHash &rh)
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

	void Router::databaseLookup(std::string const &to, std::string const &query)
	{
		ByteArray queryBytes = Base64::decode(query);
		std::array<unsigned char, 32> key;
		std::copy(queryBytes.cbegin(), queryBytes.cbegin() + 32, key.begin());

		RouterHash toHash = Base64::decode(to);
		I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(key, m_ctx.getMyRouterHash(), 0));

		m_ctx.getOutMsgDispatcher().sendMessage(toHash, dbl);
	}

	void Router::createTunnel(std::string const &to)
	{
		/*std::list<BuildRequestRecord>;
			I2NP::MessagePtr vtb(new I2NP::VariableTunnelBuild(hopList));*/
	}
}
