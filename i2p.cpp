#include "i2p.h"

#include <iostream>

#include <botan/botan.h>
#include <botan/auto_rng.h>

#include "ssu/UDPTransport.h"

namespace i2pcpp {
	I2PContext::I2PContext(std::string const &database) : m_db(database), m_inMsgDispatcher(m_jobQueue)
	{
		Botan::AutoSeeded_RNG rng;

		std::string encryptingKey = m_db.getConfigValue("private_encryption_key");
		Botan::DataSource_Memory dsm((unsigned char *)encryptingKey.data(), encryptingKey.size());
		m_encryptionKey = dynamic_cast<Botan::ElGamal_PrivateKey *>(Botan::PKCS8::load_key(dsm, rng, ""));

		std::string signingKey = m_db.getConfigValue("private_signing_key");
		Botan::DataSource_Memory dsm2((unsigned char *)signingKey.data(), signingKey.size());
		m_signingKey = dynamic_cast<Botan::DSA_PrivateKey *>(Botan::PKCS8::load_key(dsm2, rng, ""));

		std::string dsaParameters = m_db.getConfigValue("dsa_parameters");
		m_dsaParameters.PEM_decode(dsaParameters);

		Botan::BigInt encryptionKeyPublic, signingKeyPublic;
		encryptionKeyPublic = m_encryptionKey->get_y();
		signingKeyPublic = m_signingKey->get_y();

		ByteArray encryptionKeyBytes = Botan::BigInt::encode(encryptionKeyPublic), signingKeyBytes = Botan::BigInt::encode(signingKeyPublic);
		m_routerIdentity = RouterIdentity(encryptionKeyBytes, signingKeyBytes, Certificate());

		std::cerr << "My router hash: " << m_routerIdentity.getHashEncoded() << "\n";
	}
}

int main()
{
	try {
		Botan::LibraryInitializer init;
		i2pcpp::I2PContext ctx("i2p.db");

		i2pcpp::SSU::UDPTransport u(ctx, i2pcpp::Endpoint("127.0.0.1", 27333)); // TODO Pull from DB
		u.begin();

		/*		RouterInfo ri = ctx.getDatabase().getRouterInfo("zhPja0k1cboGnHbhqO50hNPTVHIRE8b4GMwi7Htey~E=");
					ByteArray rib = ri.getBytes();
					for(auto c: rib)
					cout << c;*/

		std::this_thread::sleep_for(std::chrono::seconds(5));
		u.shutdown();
	} catch (std::runtime_error &e) {
		std::cerr << "main thread exception: " << e.what() << "\n";
	}

	return 0;
}
