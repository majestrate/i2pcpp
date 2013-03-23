#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <thread>

#include <boost/asio.hpp>

#include <botan/elgamal.h>

#include "Log.h"
#include "Database.h"
#include "InboundMessageDispatcher.h"

#include "transport/Transport.h"

namespace i2pcpp {
	class Router {
		public:
			Router(std::string const &dbFile);

			~Router();

			void start();
			void stop();

			void connect(std::string const &to);

			ByteArray getRouterInfo();
			void importRouterInfo(ByteArray const &info);

			void sendRawData(std::string const &dst, std::string const &data);

		private:
			boost::asio::io_service m_ios;
			boost::asio::io_service::work m_work;
			std::thread m_serviceThread;

			Botan::ElGamal_PrivateKey *m_encryptionKey;
			Botan::DSA_PrivateKey *m_signingKey;
			RouterIdentity m_identity;

			Database m_db;
			TransportPtr m_transport;

			InboundMessageDispatcher m_inMsgDispatcher;

			i2p_logger_mt m_log;
	};
}

#endif
