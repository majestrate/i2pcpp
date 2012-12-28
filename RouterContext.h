#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterIdentity.h"
#include "Database.h"

namespace i2pcpp {
	class RouterContext {
		public:
			RouterContext(Database &db);
			~RouterContext() { if(m_signingKey) delete m_signingKey; if(m_encryptionKey) delete m_encryptionKey; }

			Database& getDatabase() { return m_db; }

			const Botan::DL_Group& getDSAParameters() const { return m_dsaParameters; }
			const Botan::DSA_PrivateKey *getSigningKey() const { return m_signingKey; }
			const Botan::ElGamal_PrivateKey *getEncryptionKey() const { return m_encryptionKey; }

			const RouterIdentity& getRouterIdentity() const { return m_routerIdentity; };

		private:
			Database& m_db;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			Botan::DL_Group m_dsaParameters;

			RouterIdentity m_routerIdentity;
	};
}

#endif
