#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <boost/asio.hpp>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterIdentity.h"
#include "tunnel/TunnelManager.h"
#include "dht/Kademlia.h"
#include "dht/SearchManager.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
#include "Signals.h"
#include "ProfileManager.h"
#include "PeerManager.h"
#include "Log.h"

namespace i2pcpp {
	class RouterContext {
		public:
			RouterContext(std::string const &dbFile, boost::asio::io_service &ios);
			RouterContext(const RouterContext &) = delete;
			RouterContext& operator=(RouterContext &) = delete;

			std::shared_ptr<const Botan::ElGamal_PrivateKey> getEncryptionKey() const;
			std::shared_ptr<const Botan::DSA_PrivateKey> getSigningKey() const;
			std::shared_ptr<const RouterIdentity> getIdentity() const;

			Database& getDatabase();

			InboundMessageDispatcher& getInMsgDisp();
			OutboundMessageDispatcher& getOutMsgDisp();

			Signals& getSignals();

			TunnelManager& getTunnelManager();
			ProfileManager& getProfileManager();
			PeerManager& getPeerManager();
			DHT::KademliaPtr getDHT();
			DHT::SearchManager& getSearchManager();

		private:
			std::shared_ptr<Botan::ElGamal_PrivateKey> m_encryptionKey;
			std::shared_ptr<Botan::DSA_PrivateKey> m_signingKey;
			std::shared_ptr<RouterIdentity> m_identity;

			Database m_db;

			TunnelManager m_tunnelManager;
			ProfileManager m_profileManager;
			PeerManager m_peerManager;

			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;

			DHT::KademliaPtr m_dht;
			DHT::SearchManager m_searchManager;

			Signals m_signals;
	};
}

#endif
