#include "Router.h"

#include <botan/elgamal.h>

/* TEMPORARY */
#include "util/Base64.h"
#include "transport/UDPTransport.h"

namespace i2pcpp {
	Router::Router(std::string const &dbFile) :
		m_work(m_ios),
		m_db(dbFile) {}

	Router::~Router()
	{
		if(m_serviceThread.joinable()) m_serviceThread.join();
	}

	void Router::start()
	{
		m_serviceThread = std::thread([&](){m_ios.run();});

		/*m_transport = TransportPtr(new UDPTransport());

			std::shared_ptr<UDPTransport> u = std::dynamic_pointer_cast<UDPTransport>(m_transport);
			u->start(Endpoint("127.0.0.1", 12345));*/
	}

	void Router::stop()
	{
		m_ios.stop();
	}

	ByteArray Router::getRouterInfo()
	{
		Botan::AutoSeeded_RNG rng;

		std::string encryptingKeyPEM = m_db.getConfigValue("private_encryption_key");
		Botan::DataSource_Memory dsm((unsigned char *)encryptingKeyPEM.data(), encryptingKeyPEM.size());
		Botan::ElGamal_PrivateKey *encryptionKey = dynamic_cast<Botan::ElGamal_PrivateKey *>(Botan::PKCS8::load_key(dsm, rng, ""));

		std::string signingKeyPEM = m_db.getConfigValue("private_signing_key");
		Botan::DataSource_Memory dsm2((unsigned char *)signingKeyPEM.data(), signingKeyPEM.size());
		Botan::DSA_PrivateKey *signingKey = dynamic_cast<Botan::DSA_PrivateKey *>(Botan::PKCS8::load_key(dsm2, rng, ""));

		Botan::BigInt encryptionKeyPublic, signingKeyPublic;
		encryptionKeyPublic = encryptionKey->get_y();
		signingKeyPublic = signingKey->get_y();

		ByteArray encryptionKeyBytes = Botan::BigInt::encode(encryptionKeyPublic), signingKeyBytes = Botan::BigInt::encode(signingKeyPublic);
		RouterIdentity identity = RouterIdentity(encryptionKeyBytes, signingKeyBytes, Certificate());

		Mapping am;
		am.setValue("caps", "BC");
		am.setValue("host", m_db.getConfigValue("ssu_external_ip"));
		am.setValue("key", identity.getHashEncoded());
		am.setValue("port", m_db.getConfigValue("ssu_external_port"));
		RouterAddress a(5, Date(0), "SSU", am);

		Mapping rm;
		rm.setValue("coreVersion", "0.9.5");
		rm.setValue("netId", "2");
		rm.setValue("router.version", "0.9.5");
		rm.setValue("stat_uptime", "90m");
		RouterInfo myInfo(identity, Date(), rm);
		myInfo.addAddress(a);
		myInfo.sign(signingKey);

		return myInfo.serialize();
	}

	void Router::importRouterInfo(ByteArray const &info)
	{
		auto begin = info.cbegin();
		m_db.setRouterInfo(RouterInfo(begin, info.cend()));
	}
}
