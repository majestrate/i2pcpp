#include "RouterContext.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
	RouterContext::RouterContext(std::string const &dbFile, boost::asio::io_service &ios) :
		m_db(dbFile),
		m_inMsgDispatcher(ios),
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

	RouterContext::~RouterContext()
	{
		if(m_encryptionKey) delete m_encryptionKey;
		if(m_signingKey) delete m_signingKey;
	}

	const Botan::ElGamal_PrivateKey *RouterContext::getEncryptionKey() const
	{
		return m_encryptionKey;
	}

	const Botan::DSA_PrivateKey *RouterContext::getSigningKey() const
	{
		return m_signingKey;
	}

	const RouterIdentity& RouterContext::getIdentity() const
	{
		return m_identity;
	}

	InboundMessageDispatcher& RouterContext::getInMsgDisp()
	{
		return m_inMsgDispatcher;
	}

	OutboundMessageDispatcher& RouterContext::getOutMsgDisp()
	{
		return m_outMsgDispatcher;
	}

	Database& RouterContext::getDatabase()
	{
		return m_db;
	}

	i2p_logger_mt& RouterContext::getLogger()
	{
		return m_log;
	}
}
