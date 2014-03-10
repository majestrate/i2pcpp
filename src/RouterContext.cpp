/**
 * @file RouterContext.cpp
 * @brief Implements RouterContext.h
 */
#include "RouterContext.h"

#include <botan/auto_rng.h>

namespace i2pcpp {
    RouterContext::RouterContext(std::string const &dbFile, boost::asio::io_service &ios) :
        m_ios(ios),
        m_db(dbFile),
        m_inMsgDispatcher(ios, *this),
        m_outMsgDispatcher(*this),
        m_signals(ios),
        m_tunnelManager(ios, *this),
        m_profileManager(*this),
        m_peerManager(ios, *this),
        m_dht(*this)
    {
        // Load the private keys from the database
        Botan::AutoSeeded_RNG rng;

        std::string encryptingKeyPEM = m_db.getConfigValue("private_encryption_key");
        Botan::DataSource_Memory dsm((unsigned char *)encryptingKeyPEM.data(), encryptingKeyPEM.size());
        m_encryptionKey = std::shared_ptr<Botan::ElGamal_PrivateKey>(dynamic_cast<Botan::ElGamal_PrivateKey *>(Botan::PKCS8::load_key(dsm, rng, (std::string)"")));

        std::string signingKeyPEM = m_db.getConfigValue("private_signing_key");
        Botan::DataSource_Memory dsm2((unsigned char *)signingKeyPEM.data(), signingKeyPEM.size());
        m_signingKey = std::shared_ptr<Botan::DSA_PrivateKey>(dynamic_cast<Botan::DSA_PrivateKey *>(Botan::PKCS8::load_key(dsm2, rng, (std::string)"")));

        Botan::BigInt encryptionKeyPublic, signingKeyPublic;
        encryptionKeyPublic = m_encryptionKey->get_y();
        signingKeyPublic = m_signingKey->get_y();

        ByteArray encryptionKeyBytes = Botan::BigInt::encode(encryptionKeyPublic), signingKeyBytes = Botan::BigInt::encode(signingKeyPublic);
        m_identity = std::make_shared<RouterIdentity>(encryptionKeyBytes, signingKeyBytes, Certificate());
    }

    std::shared_ptr<const Botan::ElGamal_PrivateKey> RouterContext::getEncryptionKey() const
    {
        return m_encryptionKey;
    }

    std::shared_ptr<const Botan::DSA_PrivateKey> RouterContext::getSigningKey() const
    {
        return m_signingKey;
    }

    std::shared_ptr<const RouterIdentity> RouterContext::getIdentity() const
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

    DHT::DHTFacade& RouterContext::getDHT()
    {
        return m_dht;
    }

    boost::asio::io_service& RouterContext::getIoService()
    {
        return m_ios;
    }
}
