/**
 * @file PeerStateList.h
 * Defines the i2pcpp::SSU::PeerStateList class.
 */
#ifndef SSUPEERSTATELIST_H
#define SSUPEERSTATELIST_H

#include <unordered_map>
#include <mutex>

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

        /**
         * Stores a list of i2pcpp::SSU::PeerState objects.
         */
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

                /**
                 * Constructs from a reference to the i2pcpp::SSU::UDPTranpport
                 *  object.
                 */
                PeerStateList(UDPTransport &transport);

                /**
                 * Adds an i2pcpp::SSU::PeerState object to the list.
                 */
                void addPeer(PeerState ps);

                /**
                 * @return the peer with a given i2cpp::Endpoint \a ep
                 * @throw std::runtime_error if the peer does not exist
                 */
                PeerState getPeer(Endpoint const &ep);

                /**
                 * @return the peer with a given i2pcpp::RouterHash \a rh
                 * @throw std::runtime_error if the peer does not exist
                 */
                PeerState getPeer(RouterHash const &rh);

                /**
                 * Deletes a peer given by its i2pcpp::Endpoint \a ep.
                 */
                void delPeer(Endpoint const &ep);

                /**
                 * Deletes a peer given by its i2pcpp::RoutherHash \a rh.
                 */
                void delPeer(RouterHash const &rh);

                /**
                 * @return true if the list contains a peer with
                 *  i2pcpp::Endpoint \a ep, false otherwise
                 */
                bool peerExists(Endpoint const &ep) const;

                /**
                 * @return true if the list contains a peer with
                 *  i2pcpp::RouterHash \a rh, false otherwise
                 */
                bool peerExists(RouterHash const &rh) const;

                /**
                 * Resets the timer of a peer given by its i2pcpp::RouterHash
                 *  \a rh.
                 * @throw std::runtime_error if there is no such peer
                 */
                void resetPeerTimer(RouterHash const &rh);

                /**
                 * @return the total number of i2pcpp::SSU::PeerState objects
                 *  stored
                 */
                uint32_t numPeers() const;

                /**
                 * @return a constant iterator to the begin of the underlying
                 *  container
                 */
                const_iterator cbegin() const;

                /**
                 * @return a constant iterator to the end of the underlying
                 *  container
                 */
                const_iterator cend() const;

                /**
                 * Called when the timer's deadline ends.
                 */
                void timerCallback(const boost::system::error_code& e, RouterHash const rh);

                /**
                 * @return the mutex for this object
                 */
                std::mutex& getMutex() const;

            private:
                UDPTransport& m_transport;
                StateContainer m_container;

                mutable std::mutex m_mutex;
        };
    }
}

#endif
