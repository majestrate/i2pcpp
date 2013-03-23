#ifndef INBOUNDMESSAGEDISPATCHER_H
#define INBOUNDMESSAGEDISPATCHER_H

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "Log.h"

namespace i2pcpp {
	class InboundMessageDispatcher {
		public:
			InboundMessageDispatcher(boost::asio::io_service &ios);

			void messageReceived(const RouterHash &from, const ByteArray &data);
			void connectionEstablished(const RouterHash &rh);

		private:
			boost::asio::io_service& m_ios;

			i2p_logger_mt m_log;
	};
}

#endif
