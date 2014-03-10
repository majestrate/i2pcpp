/**
 * @file RouterContext.h
 * @brief Defines the i2pcpp::RouterContext class.
 */
#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <boost/asio.hpp>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include <i2pcpp/datatypes/RouterIdentity.h>
#include "tunnel/TunnelManager.h"
#include "dht/DHTFacade.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
#include "Signals.h"
#include "ProfileManager.h"
#include "PeerManager.h"
#include "Log.h"

namespace i2pcpp {
    /**
     * Stores the context in which a given i2pcpp::Router runs.
     * Mainly used for storage of manager objects.
     */
    class RouterContext {
        public:
            /**
             * Constrcuts from a database file and an I/O service.
             * Loads the private keys from the database.
             * @param dbFile the file containing the database file
             * @param ios the boost::asio::io_service object of the i2pcpp::Router
             */
            RouterContext(std::string const &dbFile, boost::asio::io_service &ios);

            RouterContext(const RouterContext &) = delete;
            RouterContext& operator=(RouterContext &) = delete;

            /**
             * @return the the ElGamal private key for encryption
             */
            std::shared_ptr<const Botan::ElGamal_PrivateKey> getEncryptionKey() const;

            /**
             * @return the DSA private key for signing
             */
            std::shared_ptr<const Botan::DSA_PrivateKey> getSigningKey() const;

            /**
             * @return a pointer to the i2pcpp::RouterIdentity of this router
             */
            std::shared_ptr<const RouterIdentity> getIdentity() const;

            /**
             * @return a reference to the i2pcpp::Database object of this router
             */
            Database& getDatabase();

            /**
             * @return a reference to the i2pcpp::InboundMessageDispatcher
             */
            InboundMessageDispatcher& getInMsgDisp();

            /**
             * @return a reference to the i2pcpp::OutboundMessageDispatcher
             */
            OutboundMessageDispatcher& getOutMsgDisp();

            /**
             * @return a reference to the i2pcpp::Signals object
             */
            Signals& getSignals();

            /**
             * @return a reference to the i2pcpp::TunnelManager
             */
            TunnelManager& getTunnelManager();

            /**
             * @return a reference to the i2pcpp::ProfileManager
             */
            ProfileManager& getProfileManager();

            /**
             * @return a reference to the i2pcpp::PeerManager
             */
            PeerManager& getPeerManager();

            /**
             * @return a reference to the i2pcpp::DHT::DHTFacade
             */
            DHT::DHTFacade& getDHT();

            /**
             * @return a reference to the boost::asio::io_service object
             */
            boost::asio::io_service& getIoService();

        private:
            boost::asio::io_service& m_ios;

            /// Private key for ElGamal encryption
            std::shared_ptr<Botan::ElGamal_PrivateKey> m_encryptionKey;

            /// Private key for DSA signing
            std::shared_ptr<Botan::DSA_PrivateKey> m_signingKey;

            /// Pointer to RI object
            std::shared_ptr<RouterIdentity> m_identity;

            Database m_db;

            TunnelManager m_tunnelManager;
            ProfileManager m_profileManager;
            PeerManager m_peerManager;

            InboundMessageDispatcher m_inMsgDispatcher;
            OutboundMessageDispatcher m_outMsgDispatcher;

            DHT::DHTFacade m_dht;

            Signals m_signals;
    };
}

#endif
