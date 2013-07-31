#ifndef CHI_PEERMANAGER_H
#define CHI_PEERMANAGER_H

#include <string>
#include <unordered_map>
#include <tuple>
#include <thread>

#include <boost/asio.hpp>

#include "datatypes/RouterHash.h"

#include "PeerManager.h"
#include "Log.h"

#define MAX_FAILS 2

namespace i2pcpp {
	class RouterContext;

	class ChiPeerManager : public PeerManager {
	public:
		ChiPeerManager(boost::asio::io_service &ios, RouterContext &ctx) : PeerManager(ios,ctx) {}
		ChiPeerManager(const ChiPeerManager &) = delete;
		ChiPeerManager& operator=(ChiPeerManager &) = delete;

		void connected(const RouterHash rh);
		void failure(const RouterHash rh);
		void disconnected(const RouterHash rh);
	private:
		
		// stats for each peer
		typedef struct {
			unsigned int connect;
			unsigned int disconnect;
			unsigned int failed;
			bool shouldRemove() {
				return failed >= MAX_FAILS;
			}
		} PeerStats;
		
		std::unordered_map<std::string,PeerStats> m_stats;

	};
}

#endif
