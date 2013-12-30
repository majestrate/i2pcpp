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
                struct PeerStateContainer {
                    PeerStateContainer(PeerState const &ps) :
                        state(ps) {}
                    PeerStateContainer(PeerStateContainer &&) = default;
                    PeerStateContainer& operator=(PeerStateContainer &&) = default;

                    PeerState state;
                    std::unique_ptr<boost::asio::deadline_timer> timer;

                    Endpoint getEndpoint() const { return state.getEndpoint(); }
                    RouterHash getHash() const { return state.getHash(); }
                };

                typedef boost::multi_index_container<
                    PeerStateContainer,
                    bmi::indexed_by<
                        bmi::hashed_unique<
                            bmi::const_mem_fun<PeerStateContainer, Endpoint, &PeerStateContainer::getEndpoint>
                        >,
                        bmi::hashed_unique<
                            bmi::const_mem_fun<PeerStateContainer, RouterHash, &PeerStateContainer::getHash>
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
                void resetPeerTimer(RouterHash const &rh);
                uint32_t numPeers() const;

                const_iterator cbegin() const;
                const_iterator cend() const;

                void timerCallback(const boost::system::error_code& e, RouterHash const rh);

                std::mutex& getMutex() const;

            private:
                UDPTransport& m_transport;
                StateContainer m_container;

                mutable std::mutex m_mutex;
        };
    }
}

#endif
