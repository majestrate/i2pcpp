#ifndef SSUPEERSTATELIST_H
#define SSUPEERSTATELIST_H

#include <unordered_map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/RouterHash.h"

#include "PeerState.h"

namespace bmi = boost::multi_index;

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class PeerStateList {
			private:
				typedef boost::multi_index_container<
					PeerState,
					bmi::indexed_by<
						bmi::hashed_unique<
							bmi::const_mem_fun<PeerState, Endpoint, &PeerState::getEndpoint>
						>,
						bmi::hashed_unique<
							bmi::const_mem_fun<PeerState, RouterHash, &PeerState::getHash>
						>
					>
				> StateContainer;

			public:
				typedef StateContainer::nth_index<0>::type::const_iterator const_iterator;

				PeerStateList(UDPTransport &transport);

				void addPeer(PeerState ps);
				PeerState getPeer(Endpoint const &ep);
				PeerState getPeer(RouterHash const &rh);
				void delPeer(Endpoint const &ep);
				void delPeer(RouterHash const &rh);
				bool peerExists(Endpoint const &ep) const;
				bool peerExists(RouterHash const &rh) const;
				uint32_t numPeers() const;

				const_iterator cbegin() const;
				const_iterator cend() const;

				void timerCallback(const boost::system::error_code& e, RouterHash const &rh);

				std::mutex& getMutex() const;

			private:
				UDPTransport& m_transport;
				StateContainer m_container;

				mutable std::mutex m_mutex;
		};
	}
}

#endif
