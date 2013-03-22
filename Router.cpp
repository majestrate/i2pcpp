#include "Router.h"

/* TEMPORARY */
#include "util/Base64.h"
#include "transport/UDPTransport.h"

namespace i2pcpp {
	Router::Router(std::string const &dbFile) :
		m_work(m_ios),
		m_db(dbFile),
		m_log(boost::log::keywords::channel = "Router")
	{
		Botan::AutoSeeded_RNG rng;

		std::string encryptingKeyPEM = m_db.getConfigValue("private_encryption_key");
		Botan::DataSource_Memory dsm((unsigned char *)encryptingKeyPEM.data(), encryptingKeyPEM.size());
		m_encryptionKey = dynamic_cast<Botan::ElGamal_PrivateKey *>(Botan::PKCS8::load_key(dsm, rng, ""));

		std::string signingKeyPEM = m_db.getConfigValue("private_signing_key");
		Botan::DataSource_Memory dsm2((unsigned char *)signingKeyPEM.data(), signingKeyPEM.size());
		m_signingKey = dynamic_cast<Botan::DSA_PrivateKey *>(Botan::PKCS8::load_key(dsm2, rng, ""));

		Botan::BigInt encryptionKeyPublic, signingKeyPublic;
		encryptionKeyPublic = m_encryptionKey->get_y();
		signingKeyPublic = m_signingKey->get_y();

		ByteArray encryptionKeyBytes = Botan::BigInt::encode(encryptionKeyPublic), signingKeyBytes = Botan::BigInt::encode(signingKeyPublic);
		m_identity = RouterIdentity(encryptionKeyBytes, signingKeyBytes, Certificate());
	}

	Router::~Router()
	{
		if(m_serviceThread.joinable()) m_serviceThread.join();
		if(m_encryptionKey) delete m_encryptionKey;
		if(m_signingKey) delete m_signingKey;
	}

	void Router::start()
	{
		m_serviceThread = std::thread([&](){m_ios.run();});

		m_transport = TransportPtr(new UDPTransport(*m_signingKey, m_identity));

		std::shared_ptr<UDPTransport> u = std::dynamic_pointer_cast<UDPTransport>(m_transport);
		u->start(Endpoint(m_db.getConfigValue("ssu_bind_ip"), std::stoi(m_db.getConfigValue("ssu_bind_port"))));
	}

	void Router::stop()
	{
		m_ios.stop();
	}

	void Router::connect(std::string const &to)
	{
		BOOST_LOG_SEV(m_log, debug) << "connecting to " << to;
		RouterInfo ri = m_db.getRouterInfo(Base64::decode(to));
		std::shared_ptr<UDPTransport> u = std::dynamic_pointer_cast<UDPTransport>(m_transport);
		u->connect(ri);
	}

	ByteArray Router::getRouterInfo()
	{
		Mapping am;
		am.setValue("caps", "BC");
		am.setValue("host", m_db.getConfigValue("ssu_external_ip"));
		am.setValue("key", m_identity.getHashEncoded());
		am.setValue("port", m_db.getConfigValue("ssu_external_port"));
		RouterAddress a(5, Date(0), "SSU", am);

		Mapping rm;
		rm.setValue("coreVersion", "0.9.5");
		rm.setValue("netId", "2");
		rm.setValue("router.version", "0.9.5");
		rm.setValue("stat_uptime", "90m");
		RouterInfo myInfo(m_identity, Date(), rm);
		myInfo.addAddress(a);
		myInfo.sign(m_signingKey);

		return myInfo.serialize();
	}

	void Router::importRouterInfo(ByteArray const &info)
	{
		auto begin = info.cbegin();
		m_db.setRouterInfo(RouterInfo(begin, info.cend()));
	}
}
