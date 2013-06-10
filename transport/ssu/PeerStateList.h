#ifndef SSUPEERSTATELIST_H
#define SSUPEERSTATELIST_H

#include <unordered_map>
#include <mutex>
#include <thread>
#include "PeerState.h"

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace SSU {
		class PeerStateList {
			public:
		  
				void addRemotePeer(PeerStatePtr const &ps);
				PeerStatePtr getRemotePeer(Endpoint const &ep) const;
				PeerStatePtr getRemotePeer(RouterHash const &rh) const;
				void delRemotePeer(Endpoint const &ep);
				void delRemotePeer(RouterHash const &rh);

				bool remotePeerExists(Endpoint const &ep) const;
				bool remotePeerExists(RouterHash const &rh) const;
				uint32_t numPeers() const;

				std::unordered_map<RouterHash, PeerStatePtr>::const_iterator begin() const;
				std::unordered_map<RouterHash, PeerStatePtr>::const_iterator end() const;
			private:
				std::unordered_map<Endpoint, PeerStatePtr> m_remotePeers;
				std::unordered_map<RouterHash, PeerStatePtr> m_remotePeersByHash;
		
				mutable std::mutex m_remotePeersMutex;
		
		};
	}
}

#endif
