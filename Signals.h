#ifndef SIGNALS_H
#define SIGNALS_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "datatypes/RouterHash.h"

namespace i2pcpp {
	class Signals {
		public:
			typedef boost::signals2::signal<void(const RouterHash&)> RouterInfoSaved;

			Signals(boost::asio::io_service &ios) : m_ios(ios) {}

			void invokeRouterInfoSaved(const RouterHash &rh);

			boost::signals2::connection registerRouterInfoSaved(RouterInfoSaved::slot_type const &rish);

		private:
			boost::asio::io_service& m_ios;

			RouterInfoSaved m_routerInfoSaved;
	};
}

#endif
