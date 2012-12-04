#ifndef I2P_H
#define I2P_H

#include <string>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "Database.h"

using namespace std;
using namespace Botan;

namespace i2pcpp {
	class I2PContext {
		public:
			I2PContext(string const &database);
			~I2PContext() { if(m_signingKey) delete m_signingKey; if(m_encryptingKey) delete m_encryptingKey; }

			Database& getDatabase() { return m_db; }

			const DL_Group& getDSAParameters() const { return m_dsaParameters; }
			const DSA_PrivateKey *getSigningKey() const { return m_signingKey; }

		private:
			Database m_db;

			ElGamal_PrivateKey *m_encryptingKey;
			DSA_PrivateKey *m_signingKey;
			DL_Group m_dsaParameters;
	};
}

#endif
