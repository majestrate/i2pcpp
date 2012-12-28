#ifndef I2P_H
#define I2P_H

#include <string>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "util/LockingQueue.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"

namespace i2pcpp {
	class I2PContext {
		public:
			I2PContext(std::string const &database);
			~I2PContext() { if(m_signingKey) delete m_signingKey; if(m_encryptionKey) delete m_encryptionKey; }

			Database& getDatabase() { return m_db; }

			const Botan::DL_Group& getDSAParameters() const { return m_dsaParameters; }
			const Botan::DSA_PrivateKey *getSigningKey() const { return m_signingKey; }
			const Botan::ElGamal_PrivateKey *getEncryptionKey() const { return m_encryptionKey; }

			const RouterIdentity& getRouterIdentity() const { return m_routerIdentity; };

			InboundMessageDispatcher& getInMsgDispatcher() { return m_inMsgDispatcher; }

		private:
			Database m_db;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			Botan::DL_Group m_dsaParameters;

			RouterIdentity m_routerIdentity;

			InboundMessageDispatcher m_inMsgDispatcher;
			JobQueue m_jobQueue;
	};
}

#endif
