#include "RouterContext.h"

#include <iostream>

#include <botan/auto_rng.h>

namespace i2pcpp {
	RouterContext::RouterContext(Database &db) : m_db(db)
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
