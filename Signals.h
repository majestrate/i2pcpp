#ifndef SIGNALS_H
#define SIGNALS_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "datatypes/BuildRequestRecord.h"
#include "datatypes/RouterHash.h"

namespace i2pcpp {
	class Signals {
		public:
			typedef boost::signals2::signal<void(const RouterHash&)> RouterInfoSaved;
			typedef boost::signals2::signal<void(const std::list<BuildRequestRecord>&)> BuildTunnelRequest;

			Signals(boost::asio::io_service &ios) : m_ios(ios) {}

			void invokeRouterInfoSaved(const RouterHash &rh);
			boost::signals2::connection registerRouterInfoSaved(RouterInfoSaved::slot_type const &rish);

			void invokeBuildTunnelRequest(const std::list<BuildRequestRecord> &records);
			boost::signals2::connection registerBuildTunnelRequest(BuildTunnelRequest::slot_type const &btrh);

		private:
			boost::asio::io_service& m_ios;

			RouterInfoSaved m_routerInfoSaved;
			BuildTunnelRequest m_buildTunnelRequest;
	};
}

#endif
