#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterHash.h"
#include "datatypes/RouterIdentity.h"
#include "datatypes/RouterInfo.h"

namespace i2pcpp {
	class Database;
	class OutboundMessageDispatcher;
	class Signals;

	class RouterContext {
		public:
			RouterContext(Database &db, OutboundMessageDispatcher &omd, Signals &signals);
			~RouterContext() { if(m_signingKey) delete m_signingKey; if(m_encryptionKey) delete m_encryptionKey; }

			const Botan::DL_Group& getDSAParameters() const { return m_dsaParameters; }
			const Botan::DSA_PrivateKey *getSigningKey() const { return m_signingKey; }
			const Botan::ElGamal_PrivateKey *getEncryptionKey() const { return m_encryptionKey; }

			const RouterIdentity& getMyRouterIdentity() const { return m_routerIdentity; }
			const RouterHash& getMyRouterHash() const { return m_routerHash; }

			Database& getDatabase() const { return m_db; }
			OutboundMessageDispatcher& getOutMsgDispatcher() const { return m_outMsgDispatcher; }
			Signals& getSignals() const { return m_signals; }

		private:
			Database& m_db;
			OutboundMessageDispatcher& m_outMsgDispatcher;
			Signals &m_signals;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			Botan::DL_Group m_dsaParameters;

			RouterIdentity m_routerIdentity;
			RouterHash m_routerHash;
	};
}

#endif
