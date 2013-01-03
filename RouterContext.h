#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterHash.h"
#include "datatypes/RouterIdentity.h"
#include "datatypes/RouterInfo.h"

namespace i2pcpp {
	class Database;
	class InboundMessageDispatcher;
	class OutboundMessageDispatcher;

	class RouterContext {
		public:
			RouterContext(Database &db, InboundMessageDispatcher &imd, OutboundMessageDispatcher &omd);
			~RouterContext() { if(m_signingKey) delete m_signingKey; if(m_encryptionKey) delete m_encryptionKey; }

			const Botan::DL_Group& getDSAParameters() const { return m_dsaParameters; }
			const Botan::DSA_PrivateKey *getSigningKey() const { return m_signingKey; }
			const Botan::ElGamal_PrivateKey *getEncryptionKey() const { return m_encryptionKey; }

			const RouterIdentity& getMyRouterIdentity() const { return m_routerIdentity; };
			RouterInfo getRouterInfo(RouterHash const &rh) const;
			void setRouterInfo(RouterInfo const &info) const;

			InboundMessageDispatcher& getInMsgDispatcher() const { return m_inMsgDispatcher; }
			OutboundMessageDispatcher& getOutMsgDispatcher() const { return m_outMsgDispatcher; }

		private:
			Database& m_db;
			InboundMessageDispatcher& m_inMsgDispatcher;
			OutboundMessageDispatcher& m_outMsgDispatcher;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			Botan::DL_Group m_dsaParameters;

			RouterIdentity m_routerIdentity;
	};
}

#endif
