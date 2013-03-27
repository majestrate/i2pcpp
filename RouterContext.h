#ifndef ROUTERCONTEXT_H
#define ROUTERCONTEXT_H

#include <boost/asio.hpp>

#include <botan/elgamal.h>
#include <botan/dsa.h>

#include "datatypes/RouterIdentity.h"

#include "Database.h"
#include "InboundMessageDispatcher.h"
#include "OutboundMessageDispatcher.h"
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

			const Botan::DL_Group& getDSAParameters() const;

			i2p_logger_mt& getLogger();

		private:
			static const Botan::BigInt p;
			static const Botan::BigInt q;
			static const Botan::BigInt g;
			static const Botan::DL_Group m_group;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			RouterIdentity m_identity;

			Database m_db;

			InboundMessageDispatcher m_inMsgDispatcher;
			OutboundMessageDispatcher m_outMsgDispatcher;

			i2p_logger_mt m_log;
	};
}

#endif
