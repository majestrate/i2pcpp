#include "RouterContext.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
		const Botan::BigInt RouterContext::p = Botan::BigInt("0x9C05B2AA960D9B97B8931963C9CC9E8C3026E9B8ED92FAD0A69CC886D5BF8015FCADAE31A0AD18FAB3F01B00A358DE237655C4964AFAA2B337E96AD316B9FB1CC564B5AEC5B69A9FF6C3E4548707FEF8503D91DD8602E867E6D35D2235C1869CE2479C3B9D5401DE04E0727FB33D6511285D4CF29538D9E3B6051F5B22CC1C93");
		const Botan::BigInt RouterContext::q = Botan::BigInt("0xA5DFC28FEF4CA1E286744CD8EED9D29D684046B7");
		const Botan::BigInt RouterContext::g = Botan::BigInt("0xC1F4D27D40093B429E962D7223824E0BBC47E7C832A39236FC683AF84889581075FF9082ED32353D4374D7301CDA1D23C431F4698599DDA02451824FF369752593647CC3DDC197DE985E43D136CDCFC6BD5409CD2F450821142A5E6F8EB1C3AB5D0484B8129FCF17BCE4F7F33321C3CB3DBB14A905E7B2B3E93BE4708CBCC82");
		const Botan::DL_Group RouterContext::m_group = Botan::DL_Group(RouterContext::p, RouterContext::q, RouterContext::g);

	RouterContext::RouterContext(std::string const &dbFile, boost::asio::io_service &ios) :
		m_db(dbFile),
		m_inMsgDispatcher(ios, *this),
		m_signals(ios),
		m_tunnelManager(*this),
		m_profileManager(*this),
		m_peerManager(ios, *this)
	{
	  std::string blank = "";
		Botan::AutoSeeded_RNG rng;

		std::string encryptingKeyPEM = m_db.getConfigValue("private_encryption_key");
		Botan::DataSource_Memory dsm((unsigned char *)encryptingKeyPEM.data(), encryptingKeyPEM.size());
		m_encryptionKey = dynamic_cast<Botan::ElGamal_PrivateKey *>(Botan::PKCS8::load_key(dsm, rng, blank));

		std::string signingKeyPEM = m_db.getConfigValue("private_signing_key");
		Botan::DataSource_Memory dsm2((unsigned char *)signingKeyPEM.data(), signingKeyPEM.size());
		m_signingKey = dynamic_cast<Botan::DSA_PrivateKey *>(Botan::PKCS8::load_key(dsm2, rng, blank));

		ByteArray encryptionKeyBytes = Botan::BigInt::encode( m_encryptionKey->get_y());
		ByteArray signingKeyBytes = Botan::BigInt::encode( m_signingKey->get_y());
		
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

	Database& RouterContext::getDatabase()
	{
		return m_db;
	}

	InboundMessageDispatcher& RouterContext::getInMsgDisp()
	{
		return m_inMsgDispatcher;
	}

	OutboundMessageDispatcher& RouterContext::getOutMsgDisp()
	{
		return m_outMsgDispatcher;
	}

	Signals& RouterContext::getSignals()
	{
		return m_signals;
	}

	TunnelManager& RouterContext::getTunnelManager()
	{
		return m_tunnelManager;
	}

	ProfileManager& RouterContext::getProfileManager()
	{
		return m_profileManager;
	}

	PeerManager& RouterContext::getPeerManager()
	{
		return m_peerManager;
	}

	const Botan::DL_Group& RouterContext::getDSAParameters() const
	{
		return m_group;
	}
}
