#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <boost/asio.hpp>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterIdentity.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
#include "Signals.h"
#include "tunnel/TunnelManager.h"
#include "ProfileManager.h"
#include "PeerManager.h"
#include "Log.h"

namespace i2pcpp {
	class RouterContext {
		public:
			RouterContext(std::string const &dbFile, boost::asio::io_service &ios);
			~RouterContext();

			const Botan::ElGamal_PrivateKey *getEncryptionKey() const;
			const Botan::DSA_PrivateKey *getSigningKey() const;
			const RouterIdentity& getIdentity() const;

			Database& getDatabase();

			InboundMessageDispatcher& getInMsgDisp();
			OutboundMessageDispatcher& getOutMsgDisp();

			Signals& getSignals();

			TunnelManager& getTunnelManager();
			ProfileManager& getProfileManager();
			PeerManager& getPeerManager();

		private:
			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			RouterIdentity m_identity;

			Database m_db;

			TunnelManager m_tunnelManager;
			ProfileManager m_profileManager;
			PeerManager m_peerManager;

			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;

			Signals m_signals;
	};
}

#endif
